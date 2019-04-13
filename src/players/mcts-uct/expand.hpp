#ifndef MCTS_UCT_EXPAND_HPP
#define MCTS_UCT_EXPAND_HPP

#include <cassert>
#include <vector>
#include "../../attacks.hpp"
#include "../../cache/lru.hpp"
#include "../../display.hpp"
#include "../../makemove.hpp"
#include "../../movegen.hpp"
#include "../../position.hpp"
#include "../../zobrist.hpp"
#include "node.hpp"
#include "stack.hpp"

bool expand(cache::lru<std::uint64_t, Node> &node_cache, Stack &ss) {
    assert(ss.positions.size() > 0);

#ifndef NDEBUG
    // Make sure this node is expandable
    {
        const std::uint64_t hash = calculate_hash(ss.positions.back());
        auto entry = node_cache.get(hash);

        if (entry) {
            Node *n = entry.value();
            assert(n);
            // assert(!n->terminal_);
            // assert(!n->fully_expanded_);
        } else {
            assert(false);
        }
    }
#endif

    std::vector<Position> children;
    std::vector<Position> untried;

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

    // If the node has no possible children then it's terminal
    if (children.size() == 0) {
        const std::uint64_t parent_hash = calculate_hash(ss.positions.back());
        auto parent = node_cache.get(parent_hash);
        if (parent) {
            Node *n = parent.value();
            assert(n);
            // n->terminal_ = true;
            // n->fully_expanded_ = true;

            // assert(node_cache.get(parent_hash).value()->terminal_);
            // assert(node_cache.get(parent_hash).value()->fully_expanded_);
        } else {
            assert(false);
        }
        return false;
    }

    // Get untried children
    for (const auto &c : children) {
        const std::uint64_t child_hash = calculate_hash(c);
        if (!node_cache.has(child_hash)) {
            untried.push_back(c);
        }
    }

    // All our children have already been tried
    // even though this node claims to be expandable
    // Let's just blame transpositions
    if (untried.size() == 0) {
        const std::uint64_t parent_hash = calculate_hash(ss.positions.back());
        auto parent = node_cache.get(parent_hash);
        if (parent) {
            Node *n = parent.value();
            // n->fully_expanded_ = true;
        } else {
            assert(false);
        }

        return false;
    }

    if (untried.size() == 0) {
        const std::uint64_t parent_hash = calculate_hash(ss.positions.back());
        auto parent = node_cache.get(parent_hash);
        if (parent) {
            Node *n = parent.value();
            // std::cout << "visits: " << n->visits_ << std::endl;
            // std::cout << "reward: " << n->reward_ << std::endl;
            // std::cout << "terminal: " << n->terminal_ << std::endl;
            // std::cout << "fully expanded: " << n->fully_expanded_ <<
            // std::endl;
        } else {
        }

        display(ss.positions.back());
    }

    assert(untried.size() > 0);

    // If this is the last untried child then
    // the parent node becomes fully expanded
    if (untried.size() == 1) {
        const std::uint64_t parent_hash = calculate_hash(ss.positions.back());
        auto parent = node_cache.get(parent_hash);
        if (parent) {
            Node *n = parent.value();
            assert(n);
            // n->fully_expanded_ = true;

            // assert(node_cache.get(parent_hash).value()->fully_expanded_);
        } else {
            assert(false);
        }
    }

    // Expand random untried child
    const int index = rand() % untried.size();
    const std::uint64_t hash = calculate_hash(untried[index]);
    node_cache.add(hash, Node{});
    assert(ss.ply >= 0);
    ss.positions.push_back(untried[index]);
    ss.ply++;

    return true;
}

#endif
