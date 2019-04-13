#ifndef MCTS_UCT_BACKUP_NEGAMAX_HPP
#define MCTS_UCT_BACKUP_NEGAMAX_HPP

#include <cassert>
#include "../../cache/lru.hpp"
#include "../../zobrist.hpp"
#include "node.hpp"
#include "stack.hpp"

// Move the score of the last node back up the tree
void backup_negamax(cache::lru<std::uint64_t, Node> &node_cache,
                    const Stack &ss,
                    float delta) {
    for (auto it = ss.positions.rbegin(); it != ss.positions.rend(); ++it) {
        const std::uint64_t hash = calculate_hash(*it);
        auto node = node_cache.get(hash);

        assert(node);
        assert(node.value());

        Node new_node = *node.value();
        new_node.visits_ += 1;
        new_node.reward_ += delta;
        assert(new_node.visits_ > 0);
        node_cache.add(hash, new_node);
        delta = 1.0 - delta;
    }
}

#endif
