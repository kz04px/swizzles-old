#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>
#include "../../cache/lru.hpp"
#include "../../pv.hpp"
#include "../all.hpp"
#include "backup_negamax.hpp"
#include "best_child.hpp"
#include "default_policy.hpp"
#include "expand.hpp"
#include "node.hpp"
#include "pv.hpp"
#include "stack.hpp"
#include "tree_policy.hpp"
#include "ucb.hpp"

cache::lru<std::uint64_t, Node> node_cache(10000);

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

    const std::uint64_t hash = calculate_hash(pos);
    auto root = node_cache.get(hash);

    // Add the root node to the cache if it's not there already
    if (!root) {
        node_cache.add(hash, Node{});
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto end = start;

    switch (options.type) {
        case SearchType::Time:
            options.nodes = std::numeric_limits<std::uint64_t>::max();
            if (pos.flipped) {
                end += std::chrono::milliseconds(options.btime /
                                                 options.movestogo);
            } else {
                end += std::chrono::milliseconds(options.wtime /
                                                 options.movestogo);
            }
            break;
        case SearchType::Movetime:
            options.nodes = std::numeric_limits<std::uint64_t>::max();
            end += std::chrono::milliseconds(options.movetime);
            break;
        case SearchType::Nodes:
            end += std::chrono::hours(1);
            break;
        case SearchType::Infinite:
            options.nodes = std::numeric_limits<std::uint64_t>::max();
            end += std::chrono::hours(1);
            break;
        default:
            options.nodes = std::numeric_limits<std::uint64_t>::max();
            end += std::chrono::milliseconds(1000);
            break;
    }
    /*
        std::cout << "Size: " << node_cache.max_size() * sizeof(Node) / 1024 /
       1024
                  << "MB" << std::endl;
    */
    // Prepare search stack
    Stack ss;

    int seldepth = 0;

    std::cout << "info string iteration" << std::endl;
    std::uint64_t nodes = 0ULL;
    while (nodes < options.nodes && stop == false &&
           std::chrono::high_resolution_clock::now() < end) {
        assert(node_cache.has(hash));

        // Clear search stack
        ss.positions.clear();
        ss.moves.clear();

        // We start with only the root position in the stack
        ss.ply = 1;
        ss.positions.push_back(pos);

        tree_policy(node_cache, ss, 0.1);
        assert(ss.ply > 0);
        const float delta = default_policy(ss.positions.back());
        backup_negamax(node_cache, ss, delta);

        nodes++;

        if (nodes == 1 || nodes % 10000 == 0) {
            PV pv = get_pv(node_cache, pos);
            assert(pv.length > 0);
            assert(pv.legal(pos));

            seldepth = std::max(ss.ply, seldepth);

            const std::uint64_t hash = calculate_hash(pos);
            auto entry = node_cache.get(hash);

            if (!entry) {
                continue;
            }

            Node *n = entry.value();
            assert(n);
            assert(n->visits_ > 0);

            // Timing
            auto now = std::chrono::high_resolution_clock::now();
            auto diff = now - start;
            auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(diff);

            std::cout << "info";
            std::cout << " nodes " << nodes;
            std::cout << " time " << elapsed.count();
            std::cout << " score " << std::setprecision(2) << std::fixed
                      << 100 * (float)n->reward_ / n->visits_ << "%";
            if (elapsed.count() > 0) {
                std::cout << " nps "
                          << static_cast<std::uint64_t>(
                                 nodes / (elapsed.count() / 1000.0));
            }
            std::cout << " cache " << std::setprecision(2) << std::fixed
                      << 100.0 * node_cache.size() / node_cache.max_size()
                      << "%";
            std::cout << " seldepth " << seldepth;
            if (pv.length > 0) {
                std::cout << " pv " << pv.string(pos.flipped);
            }
            std::cout << std::endl;
        }
    }

#ifndef NDEBUG
    // Print the top children
    std::cout << "move   score     visits  pv" << std::endl;
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

            // Details
            PV pv = get_pv(node_cache, npos);
            assert(pv.legal(npos));
            float score = 100 * (float)n->reward_ / n->visits_;

            // Print -- move number
            // std::cout << std::left << std::setw(3) << a + 1 << " ";

            // Print -- move
            std::cout << std::left << std::setw(6)
                      << move_uci(moves[i], pos.flipped);

            // Print -- score
            std::cout << std::right << std::setw(6) << std::fixed
                      << std::setprecision(2) << score << "%";

            // Print -- visits
            std::cout << std::right << std::setw(10) << n->visits_;

            // Print -- pv
            if (pv.length > 0) {
                std::cout << "  " << pv.string(npos.flipped);
            }
            std::cout << std::endl;
        }
    }
#endif

    PV pv = get_pv(node_cache, pos);
    assert(pv.length > 0);
    assert(pv.legal(pos));

    if (pv.length > 0) {
        std::cout << "bestmove " << move_uci(pv.moves[0], pos.flipped)
                  << std::endl;
    } else {
        assert(false);
        std::cout << "bestmove 0000" << std::endl;
    }
}

}  // namespace player
