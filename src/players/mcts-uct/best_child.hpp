#ifndef MCTS_UCT_BEST_CHILD_HPP
#define MCTS_UCT_BEST_CHILD_HPP

#include <cassert>
#include <cmath>
#include <vector>
#include "../../attacks.hpp"
#include "../../cache/lru.hpp"
#include "../../display.hpp"
#include "../../makemove.hpp"
#include "../../movegen.hpp"
#include "../../position.hpp"
#include "stack.hpp"
#include "ucb.hpp"

// Insert the best child of the last ss position into the ss
bool best_child(cache::lru<std::uint64_t, Node> &node_cache,
                Stack &ss,
                const float cp) {
    assert(ss.positions.size() > 0);
    float best_score = std::numeric_limits<float>::lowest();
    std::vector<Position> children;
    std::vector<Position> best_children;

    // Get child positions
    Move moves[MAX_MOVES];
    int num_moves = movegen(ss.positions.back(), moves);
    for (int i = 0; i < num_moves; ++i) {
        Position npos = ss.positions.back();
        make_move(npos, moves[i]);
        if (check(npos, Colour::THEM)) {
            continue;
        }
        children.push_back(npos);
    }

    if (children.size() == 0) {
        return false;
    }

    assert(children.size() > 0);

    const std::uint64_t parent_hash = calculate_hash(ss.positions.back());
    auto parent_node = node_cache.get(parent_hash);

    assert(parent_node);
    assert(parent_node.value());

    // Score children
    for (const auto &c : children) {
        // Get child node
        const std::uint64_t child_hash = calculate_hash(c);
        auto child_node = node_cache.get(child_hash);

        // Skip if the child isn't in the cache
        // FIXME:
        if (!child_node) {
            continue;
        }

        assert(parent_node);
        assert(child_node);
        assert(parent_node.value());
        assert(child_node.value());

        // Score child node
        const float score = ucb(parent_node.value(), child_node.value(), cp);
        assert(!std::isnan(score));

        // Update best score
        if (score > best_score) {
            best_score = score;
            best_children.clear();
            best_children.push_back(c);
        } else if (score == best_score) {
            best_children.push_back(c);
        }
    }

    if (best_children.size() == 0) {
        display(ss.positions.back());
        std::cout << "best_children.size() " << best_children.size()
                  << std::endl;
        std::cout << "children.size() " << children.size() << std::endl;
    }

    assert(best_children.size() > 0);
    assert(best_score > std::numeric_limits<float>::lowest());

    // Choose a child from the top scorers at random
    const int index = rand() % best_children.size();

    // Add child to the search stack
    assert(ss.ply >= 0);
    assert(ss.ply < MAX_DEPTH);
    ss.positions.push_back(best_children[index]);
    ss.ply++;

    return true;
}

#endif
