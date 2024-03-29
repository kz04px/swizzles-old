#include <chrono>
#include <climits>
#include <cstdint>
#include <cstring>
#include <iostream>
#include "../../assert.hpp"
#include "../../attacks.hpp"
#include "../../display.hpp"
#include "../../makemove.hpp"
#include "../../movegen.hpp"
#include "../../pv.hpp"
#include "../all.hpp"
#include "alphabeta.hpp"

using clockz = std::chrono::high_resolution_clock;
using ms = std::chrono::milliseconds;

namespace player {

void alphabeta(const Position &pos,
               Hashtable &tt,
               bool &stop,
               SearchOptions options) {
    SearchInfo info;
    info.nodes = 0ULL;
    info.leafnodes = 0ULL;
    info.seldepth = 0;
    info.start = clockz::now();
    info.stop = &stop;
    info.tt = &tt;
#ifndef NDEBUG
    for (int i = 0; i < MAX_MOVES; ++i) {
        info.cutoffs[i] = 0ULL;
    }
#endif

    SearchStack ss[MAX_DEPTH + 1];
    for (int i = 0; i < MAX_DEPTH + 1; ++i) {
        ss[i].ply = i;
        ss[i].nullmove = true;
        ss[i].killer1 = NO_MOVE;
        ss[i].killer2 = NO_MOVE;
    }

    options.depth = std::max(1, options.depth);
    options.movetime = std::max(1, options.movetime);
    options.movestogo = std::max(1, options.movestogo);
    options.wtime = std::max(1, options.wtime);
    options.btime = std::max(1, options.btime);
    options.winc = std::max(0, options.binc);
    options.binc = std::max(0, options.binc);

    switch (options.type) {
        case SearchType::Time:
            options.depth = std::numeric_limits<int>::max();
            options.nodes = std::numeric_limits<uint64_t>::max();
            if (pos.flipped) {
                const int use = (float)options.btime / options.movestogo;
                info.end = info.start + ms(use);
            } else {
                const int use = (float)options.wtime / options.movestogo;
                info.end = info.start + ms(use);
            }
            break;
        case SearchType::Movetime:
            options.depth = std::numeric_limits<int>::max();
            options.nodes = std::numeric_limits<uint64_t>::max();
            info.end = info.start + ms(options.movetime);
            break;
        case SearchType::Depth:
            options.nodes = std::numeric_limits<uint64_t>::max();
            info.end = info.start + std::chrono::hours(24);
            break;
        case SearchType::Nodes:
            options.depth = std::numeric_limits<int>::max();
            info.end = info.start + std::chrono::hours(24);
            break;
        case SearchType::Infinite:
            options.nodes = std::numeric_limits<uint64_t>::max();
            options.depth = std::numeric_limits<int>::max();
            info.end = info.start + std::chrono::hours(24);
            break;
        default:
            options.depth = std::numeric_limits<int>::max();
            options.nodes = std::numeric_limits<uint64_t>::max();
            info.end = info.start + ms(1000);
            break;
    }

    PV pv;
    for (int d = 1; d <= options.depth && d < MAX_DEPTH; ++d) {
        PV npv;

        int score = 0;
        if (d <= 3) {
            score = alphabeta(pos, info, ss, npv, -INF, INF, d);
        } else {
            for (auto &b : {50, 200, INF}) {
                score = alphabeta(pos, info, ss, npv, -b, b, d);

                if (-b < score && score < b) {
                    break;
                }
            }
        }

        const auto now = clockz::now();

        // If we ran out of time or were asked to stop, discard the result
        if (d > 1 && (now >= info.end || *info.stop == true)) {
            break;
        }

        memcpy(pv.moves, npv.moves, npv.length * sizeof(Move));
        pv.length = npv.length;

        UCI_ASSERT(pv.length > 0);
        UCI_ASSERT(pv.legal(pos) == true);

        const auto time_taken =
            std::chrono::duration_cast<ms>(now - info.start);

        std::cout << "info";
        std::cout << " depth " << d;
        std::cout << " seldepth " << info.seldepth;

        if (score > INF - MAX_DEPTH) {
            std::cout << " score mate " << (INF - score + 1) / 2;
        } else if (score < -INF + MAX_DEPTH) {
            std::cout << " score mate " << (-INF - score - 1) / 2;
        } else {
            std::cout << " score cp " << score;
        }

        std::cout << " nodes " << info.nodes;
        if (time_taken.count() > 0) {
            std::cout << " nps " << 1000 * int(info.nodes / time_taken.count());
        }
        std::cout << " time " << time_taken.count();

        if (pv.length > 0) {
            std::cout << " pv " + pv.string(pos.flipped);
        }

        std::cout << std::endl;

        /*
        // Try and predict if we have enough time for the next iteration
        if (info.start + 4 * time_taken * CLOCKS_PER_SEC > info.end) {
            break;
        }
        */
    }

#ifndef NDEBUG
    std::cout << "info string TT hits " << info.hash_hits << std::endl;
    std::cout << "info string TT collisions " << info.hash_collisions
              << std::endl;

    uint64_t sum = 0ULL;
    for (int i = 0; i < MAX_MOVES; ++i) {
        sum += info.cutoffs[i];
    }
    for (int i = 0; i < 8; ++i) {
        std::cout << "info string move " << i + 1 << " beta cutoffs "
                  << info.cutoffs[i] << " ";
        if (sum > 0ULL) {
            std::cout << "(" << 100.0 * (double)info.cutoffs[i] / sum << "%)";
        } else {
            std::cout << "-";
        }
        std::cout << std::endl;
    }
#endif

    if (pv.length > 0) {
        std::cout << "bestmove " << pv.moves[0].uci(pos.flipped) << std::endl;
    } else {
        // Ideally we should never find any legal moves here
        // if it does happen then there's almost certainly a bug somewhere
        // but playing the found move will be better than making a null move
        Move best_move = NO_MOVE;
        Move moves[MAX_MOVES];
        int num_moves = movegen(pos, moves);

        for (int i = 0; i < num_moves; ++i) {
            Position npos = pos;
            make_move(npos, moves[i]);
            if (check(npos, Colour::THEM) == true) {
                continue;
            }

            best_move = moves[i];
            break;
        }

        UCI_ASSERT(best_move == NO_MOVE);

        if (best_move != NO_MOVE) {
            std::cout << "bestmove " << best_move.uci(pos.flipped) << std::endl;
        } else {
            std::cout << "bestmove 0000" << std::endl;
        }
    }
}

}  // namespace player
