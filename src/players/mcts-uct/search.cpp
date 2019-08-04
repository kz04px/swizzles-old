#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include "../../assert.hpp"
#include "../../attacks.hpp"
#include "../../eval.hpp"
#include "../../pv.hpp"
#include "../../rollout.hpp"
#include "../all.hpp"
#include "node.hpp"

#define SIGMOID(x) (1.0 / (1.0 + exp(-x)))

float score_ucb(const State &p, const State &c, const float cp) {
    UCI_ASSERT(c.visits_ > 0);
    UCI_ASSERT(p.visits_ > 0);
    const float exploitation = c.reward_ / c.visits_;
    const float exploration = cp * sqrt(2.0 * log(p.visits_) / c.visits_);
    return exploitation + exploration;
}

Node *best_child(Node *n, const float cp) {
    UCI_ASSERT(n);
    float best_score = std::numeric_limits<float>::lowest();
    std::vector<Node *> best_children;

    for (auto &c : n->children_) {
        const float score = score_ucb(n->state_, c.state_, cp);
        if (score > best_score) {
            best_score = score;
            best_children.clear();
            best_children.push_back(&c);
        } else if (score == best_score) {
            best_children.push_back(&c);
        }
    }

    UCI_ASSERT(best_children.size() > 0);
    UCI_ASSERT(best_score > std::numeric_limits<float>::lowest());

    // Choose a child from the top scorers at random
    int index = rand() % best_children.size();

    return best_children[index];
}

float default_policy(const State &s) {
    float score = 0.0;

    // Game over
    if (s.moves_.size() == 0) {
        const bool in_check = check(s.pos_, Colour::US);
        if (in_check) {
            score = -INF;
        } else {
            score = 0;
        }
    }
    // Draw score
    else if (is_fifty_moves(s.pos_) || repetitions(s.pos_) == 2) {
        score = 0.0;
    }
    // Eval
    else {
        score = 0.001 * eval(s.pos_);
    }

    return 1.0 - SIGMOID(score);
}

Node *tree_policy(Node *n, const float cp) {
    UCI_ASSERT(n);
    while (!n->terminal()) {
        if (!n->fully_expanded()) {
            UCI_ASSERT(n);
            return n->expand();
        }
        n = best_child(n, cp);
    }
    return n;
}

void backup_negamax(Node *n, float delta) {
    UCI_ASSERT(n);
    while (n) {
        n->state_.visits_ += 1;
        n->state_.reward_ += delta;
        delta = 1.0 - delta;
        n = n->parent_;
    }
}

PV get_pv(Node *node) {
    UCI_ASSERT(node);

    PV pv;
    while (node->children_.size() > 0) {
        int best_index = 0;
        float best_score = std::numeric_limits<float>::lowest();

        for (unsigned int n = 0; n < node->children_.size(); ++n) {
            UCI_ASSERT(node->children_[n].state_.visits_ > 0);

            const float score = node->children_[n].state_.visits_;

            if (score >= best_score) {
                best_index = n;
                best_score = score;
            }
        }

        if (pv.length < 8) {
            pv.moves[pv.length] = node->children_[best_index].state_.move_;
            pv.length++;
        }

        // Move on to the best child node
        node = &(node->children_[best_index]);
    }

    return pv;
}

namespace player {

void mcts(const Position &pos,
          Hashtable &tt,
          bool &stop,
          SearchOptions options) {
    // No moves possible if the game is already over
    if (is_fifty_moves(pos) || repetitions(pos) == 2) {
        std::cout << "bestmove 0000" << std::endl;
        return;
    }

    Node root(pos, NO_MOVE, nullptr);

    auto start = std::chrono::high_resolution_clock::now();
    auto end = start;

    switch (options.type) {
        case SearchType::Time:
            options.nodes = std::numeric_limits<uint64_t>::max();
            if (pos.flipped) {
                end += std::chrono::milliseconds(options.btime /
                                                 options.movestogo);
            } else {
                end += std::chrono::milliseconds(options.wtime /
                                                 options.movestogo);
            }
            break;
        case SearchType::Movetime:
            options.nodes = std::numeric_limits<uint64_t>::max();
            end += std::chrono::milliseconds(options.movetime);
            break;
        case SearchType::Nodes:
            end += std::chrono::hours(1);
            break;
        case SearchType::Infinite:
            options.nodes = std::numeric_limits<uint64_t>::max();
            end += std::chrono::hours(1);
            break;
        default:
            options.nodes = std::numeric_limits<uint64_t>::max();
            end += std::chrono::milliseconds(1000);
            break;
    }

    uint64_t nodes = 0ULL;
    while (nodes < options.nodes && stop == false &&
           std::chrono::high_resolution_clock::now() < end) {
        Node *n = tree_policy(&root, 0.1);
        UCI_ASSERT(n);
        const float delta = default_policy(n->state_);
        backup_negamax(n, delta);

        nodes++;

        if (nodes == 1 || nodes % 10000 == 0) {
            PV pv = get_pv(&root);
            UCI_ASSERT(pv.length > 0);
            UCI_ASSERT(pv.legal(pos));

            // Timing
            auto now = std::chrono::high_resolution_clock::now();
            auto diff = now - start;
            auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(diff);

            std::cout << "info";
            std::cout << " nodes " << nodes;
            std::cout << " time " << elapsed.count();
            std::cout << " score " << std::setprecision(4)
                      << 100 * (float)root.state_.reward_ / root.state_.visits_
                      << "%";
            if (elapsed.count() / 1000 > 0) {
                std::cout << " nps " << nodes / (elapsed.count() / 1000);
            }
            if (pv.length > 0) {
                std::cout << " pv " << pv.string(pos.flipped);
            }
            std::cout << std::endl;
        }
    }

#ifndef NDEBUG
    // Print the top children
    std::vector<Node *> indices;
    for (auto &c : root.children_) {
        indices.push_back(&c);
    }
    std::cout << "N   move   score     visits  pv" << std::endl;
    for (unsigned int a = 0; a < indices.size(); ++a) {
        // Score
        int best_score = indices[a]->state_.visits_;
        int index = a;
        for (unsigned int b = a + 1; b < indices.size(); ++b) {
            int score = indices[b]->state_.visits_;
            if (score > best_score) {
                best_score = score;
                index = b;
            }
        }

        // Swap
        Node *store = indices[a];
        indices[a] = indices[index];
        indices[index] = store;

        // Details
        PV pv = get_pv(indices[a]);
        UCI_ASSERT(pv.legal(indices[a]->state_.pos_));
        float score = 100 * (float)indices[a]->state_.reward_ /
                      indices[a]->state_.visits_;

        // Print -- move number
        std::cout << std::left << std::setw(3) << a + 1 << " ";

        // Print -- move
        std::cout << std::left << std::setw(6)
                  << indices[a]->state_.move_.uci(pos.flipped);

        // Print -- score
        std::cout << std::right << std::setw(6) << std::fixed
                  << std::setprecision(2) << score << "%";

        // Print -- visits
        std::cout << std::right << std::setw(10) << indices[a]->state_.visits_;

        // Print -- pv
        if (pv.length > 0) {
            std::cout << "  " << pv.string(indices[a]->state_.pos_.flipped);
        }

        std::cout << std::endl;
    }
#endif

    PV pv = get_pv(&root);
    UCI_ASSERT(pv.length > 0);
    UCI_ASSERT(pv.legal(pos));

    if (pv.length > 0) {
        std::cout << "bestmove " << pv.moves[0].uci(pos.flipped) << std::endl;
    } else {
        UCI_ASSERT(false);
        std::cout << "bestmove 0000" << std::endl;
    }
}

}  // namespace player
