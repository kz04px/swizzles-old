#ifndef MCTS_UCT_PV_HPP
#define MCTS_UCT_PV_HPP

#include <cassert>
#include <limits>
#include <vector>
#include "../../attacks.hpp"
#include "../../cache/lru.hpp"
#include "../../makemove.hpp"
#include "../../move.hpp"
#include "../../movegen.hpp"
#include "../../position.hpp"
#include "node.hpp"
#include "pv.hpp"

PV get_pv(cache::lru<std::uint64_t, Node> &node_cache,
          const Position &root_pos) {
    Position pos = root_pos;
    PV pv;
    while (pv.length < 8) {
        float best_score = std::numeric_limits<float>::lowest();
        Move best_move = NO_MOVE;

        // Get child positions
        Move moves[MAX_MOVES];
        int num_moves = movegen(pos, moves);
        for (int i = 0; i < num_moves; ++i) {
            Position npos = pos;
            make_move(npos, moves[i]);
            if (check(npos, Colour::THEM)) {
                continue;
            }

            const std::uint64_t hash = calculate_hash(npos);
            auto entry = node_cache.get(hash);

            if (entry) {
                Node *n = entry.value();
                assert(n);
                const float score = n->visits_;
                if (score > best_score) {
                    best_score = score;
                    best_move = moves[i];
                }
            }
        }

        if (best_move == NO_MOVE) {
            break;
        }

        assert(best_score > std::numeric_limits<float>::lowest());
        assert(best_move != NO_MOVE);

        pv.moves[pv.length] = best_move;
        pv.length++;

        make_move(pos, best_move);
    }

    return pv;
}

#endif
