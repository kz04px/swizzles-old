#include "mcts-uct.hpp"
#include <cassert>
#include <climits>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include "attacks.hpp"
#include "eval.hpp"
#include "hashtable.hpp"
#include "makemove.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "options.hpp"
#include "pv.hpp"
#include "rollout.hpp"

struct State {
   public:
    State() : pos(), num_moves(0), curr_move(0), moves{} {
    }

    State(Position position)
        : pos(position), num_moves(0), curr_move(0), moves{} {
        assert(legal_position(position) == true);
        num_moves = movegen(position, moves);

        // FIXME:
        // This is a dirty fix because we don't have legal movegen
        for (int i = 0; i < num_moves; ++i) {
            Position npos = position;
            make_move(npos, moves[i]);
            if (check(npos, Colour::THEM) == true) {
                moves[i] = moves[num_moves - 1];
                num_moves--;
                i--;
                continue;
            }
        }

        curr_move = 0;
    }

    int moves_left() const {
        assert(num_moves - curr_move >= 0);

        return num_moves - curr_move;
    }

    Move next() {
        assert(curr_move < num_moves);

        curr_move++;

        assert(curr_move - 1 >= 0);
        assert(curr_move - 1 < num_moves);

        return moves[curr_move - 1];
    }

    void move(const Move &move) {
        make_move(pos, move);
    }

    Position pos;
    int num_moves;
    int curr_move;
    Move moves[MAX_MOVES];
};

struct Node {
   public:
    Node(State s, Node *p, Move m)
        : state(s), move(m), score(0.0), visits(0), parent(p), children({}) {
    }

    Node *expand() {
        assert(state.moves_left() > 0);

        Move move = state.next();
        Position npos = state.pos;
        make_move(npos, move);

        State nstate = State(npos);
        Node child = Node(nstate, this, move);
        children.push_back(child);
        return &children[children.size() - 1];
    }

    bool terminal() const {
        return children.size() == 0 && state.moves_left() == 0;
    }

    State state;
    Move move;
    float score;
    int visits;
    Node *parent;
    std::vector<Node> children;
};

Node *best_child(Node *node, const float c) {
    assert(node);
    assert(node->visits > 0);
    assert(node->children.size() > 0);
    assert(node->state.moves_left() == 0);

    float best = std::numeric_limits<float>::lowest();
    std::vector<Node *> best_children;

    for (Node &child : node->children) {
        assert(child.visits > 0);
        assert(child.score >= 0);
        assert(child.score <= child.visits);

        const float exploitation = child.score / child.visits;
        const float expansion =
            c * sqrt(2.0 * log(node->visits) / child.visits);
        const float evaluation = -(float)eval(child.state.pos) / 1000.0;
        const float score = exploitation + expansion + evaluation;

        if (score > best) {
            best = score;
            best_children.clear();
            best_children.push_back(&child);
        } else if (score == best) {
            best_children.push_back(&child);
        }
    }

    assert(best != std::numeric_limits<float>::lowest());
    assert(best_children.size() > 0);
    assert(best_children.size() <= node->children.size());

    int index = rand() % best_children.size();

    return best_children[index];
}

Node *tree_policy(Node *node) {
    assert(node);

    while (node->terminal() == false) {
        if (node->state.moves_left() > 0) {
            node = node->expand();
            break;
        } else {
            node = best_child(node, 0.1);
        }
    }

    assert(node);

    return node;
}

float default_policy(Hashtable &tt, const Position &pos) {
    return 1.0 - rollout(pos, 400);
}

void backup_negamax(Node *node, float delta) {
    assert(node);
    assert(0.0 <= delta);
    assert(delta <= 1.0);

    while (node) {
        node->visits += 1;
        node->score += delta;
        delta = 1.0 - delta;
        node = node->parent;
    }
}

PV get_pv(Node *node) {
    assert(node);

    PV pv;
    while (node->children.size() > 0) {
        int best_index = 0;
        float best_score = std::numeric_limits<float>::lowest();

        for (unsigned int n = 0; n < node->children.size(); ++n) {
            assert(node->children[n].visits > 0);

            const float score = node->children[n].visits;

            if (score >= best_score) {
                best_index = n;
                best_score = score;
            }
        }

        if (pv.length < 8) {
            pv.moves[pv.length] = node->children[best_index].move;
            pv.length++;
        }

        // Move on to the best child node
        node = &(node->children[best_index]);
    }

    return pv;
}

void print_tree(Node node, int depth) {
    if (depth > 8) {
        return;
    }

    for (auto &child : node.children) {
        float score = child.score / child.visits +
                      (0.0) * sqrt(2.0 * log(node.visits) / child.visits);
        for (int i = 0; i < depth; ++i) {
            std::cout << "            ";
        }
        std::cout << move_uci(child.move, child.state.pos.flipped) << " ("
                  << child.score << "/" << child.visits << " " << score << ")"
                  << std::endl;

        if (child.children.size() > 0) {
            print_tree(child, depth + 1);
        }
    }
}

void mcts_uct(const Position &pos,
              Hashtable &tt,
              bool &stop,
              SearchOptions options) {
    Node root = Node(pos, nullptr, NO_MOVE);
    uint64_t iteration = 0;

    clock_t start = clock();
    clock_t end_target = clock();

    options.movetime = std::max(1, options.movetime);
    options.movestogo = std::max(1, options.movestogo);
    options.wtime = std::max(1, options.wtime);
    options.btime = std::max(1, options.btime);
    options.winc = std::max(0, options.binc);
    options.binc = std::max(0, options.binc);

    switch (options.type) {
        case SearchType::Time:
            options.nodes = std::numeric_limits<uint64_t>::max();
            if (pos.flipped) {
                end_target =
                    start +
                    ((double)(options.btime / options.movestogo) / 1000.0) *
                        CLOCKS_PER_SEC;
            } else {
                end_target =
                    start +
                    ((double)(options.wtime / options.movestogo) / 1000.0) *
                        CLOCKS_PER_SEC;
            }
            break;
        case SearchType::Movetime:
            options.nodes = std::numeric_limits<uint64_t>::max();
            end_target =
                start + ((double)options.movetime / 1000.0) * CLOCKS_PER_SEC;
            break;
        case SearchType::Nodes:
            end_target = std::numeric_limits<clock_t>::max();
            break;
        case SearchType::Infinite:
            options.nodes = std::numeric_limits<uint64_t>::max();
            end_target = std::numeric_limits<clock_t>::max();
            break;
        default:
            options.nodes = std::numeric_limits<int>::max();
            end_target = start + 1.0 * CLOCKS_PER_SEC;
            break;
    }

    while (iteration <= options.nodes && stop == false &&
           clock() <= end_target) {
        Node *node = tree_policy(&root);
        float score = default_policy(tt, node->state.pos);
        backup_negamax(node, score);
        iteration++;

        if (iteration == 1 || iteration % 5000 == 0) {
            double time = (double)(clock() - start) / CLOCKS_PER_SEC;
            int last_best = std::numeric_limits<int>::max();
            for (int i = 0; i < options::spins["MultiPV"].val_; ++i) {
                Node *best_node = nullptr;
                int current_best = std::numeric_limits<int>::lowest();
                for (auto &node : root.children) {
                    if (node.visits > current_best && node.visits < last_best) {
                        best_node = &node;
                        current_best = node.visits;
                    }
                }

                if (current_best == std::numeric_limits<int>::lowest()) {
                    break;
                }

                assert(best_node);

                if (best_node->visits == 0) {
                    break;
                }

                last_best = current_best;

                PV pv = get_pv(best_node);
                assert(pv.legal(best_node->state.pos) == true);
                std::string pv_string = pv.string(best_node->state.pos.flipped);

                std::cout << "info"
                          << " multipv " << i + 1 << " nodes " << iteration
                          << " score "
                          << 100.0 * best_node->score / best_node->visits << "%"
                          << " visits " << best_node->visits << " time "
                          << (uint64_t)(1000.0 * time);
                if (time > 0.0) {
                    std::cout << " nps " << (uint64_t)(root.visits / time);
                }
                std::cout << " pv " << move_uci(best_node->move, pos.flipped);
                if (pv.length > 0) {
                    std::cout << " " + pv_string;
                }
                std::cout << std::endl;
            }
        }

        // print_tree(root, 0);
        // std::cout << "~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    }

    // Extract PV
    PV pv = get_pv(&root);
    assert(pv.legal(pos) == true);

    if (pv.length > 0) {
        std::cout << "bestmove " << move_uci(pv.moves[0], pos.flipped)
                  << std::endl;
    } else {
        // Play a random move
        Move moves[MAX_MOVES];
        int num_moves = movegen(pos, moves);

        // Remove illegal moves
        for (int i = 0; i < num_moves; ++i) {
            Position npos = pos;
            make_move(npos, moves[i]);
            if (check(npos, Colour::THEM) == true) {
                moves[i] = moves[num_moves - 1];
                num_moves--;
                i--;
                continue;
            }
        }

        if (num_moves > 0) {
            assert(false);
            int n = rand() % num_moves;

            std::cout << "bestmove " << move_uci(moves[n], pos.flipped)
                      << std::endl;
        } else {
            std::cout << "bestmove 0000" << std::endl;
        }
    }

    // print_tree(root, 0);
}
