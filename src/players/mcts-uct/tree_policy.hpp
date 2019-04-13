#ifndef MCTS_UCT_TREE_POLICY_HPP
#define MCTS_UCT_TREE_POLICY_HPP

#include <cassert>
#include "../../cache/lru.hpp"
#include "../../position.hpp"
#include "best_child.hpp"
#include "stack.hpp"

// Do stuff
void tree_policy(cache::lru<std::uint64_t, Node> &node_cache,
                 Stack &ss,
                 const float cp) {
    assert(ss.ply == 1);
    assert(ss.positions.size() == 1);
    while (ss.ply < MAX_DEPTH) {
        assert(ss.ply > 0);
        assert(ss.positions.size() > 0);
        const std::uint64_t hash = calculate_hash(ss.positions.back());

        // Probe cache
        auto entry = node_cache.get(hash);

        // If the position is in the cache
        if (entry) {
            Node *n = entry.value();
            assert(n);

            // Try expand our node
            const bool a = expand(node_cache, ss);
            if (a) {
                return;
            }

            // Find the node's best child and continue
            const bool b = best_child(node_cache, ss, cp);
            if (!b) {
                return;
            }
        }
        // If the position isn't in the cache
        // then we need to add and return it
        else {
            // node_cache.add(hash, Node{});
            // return node_cache.get(hash).value();
            assert(false);
        }
    }
}

#endif
