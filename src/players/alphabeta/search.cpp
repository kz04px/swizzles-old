#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <iostream>
#include "../../attacks.hpp"
#include "../../display.hpp"
#include "../../makemove.hpp"
#include "../../movegen.hpp"
#include "../../pv.hpp"
#include "../all.hpp"
#include "alphabeta.hpp"

namespace player {

void alphabeta(const Position &pos,
               Hashtable &tt,
               bool &stop,
               SearchOptions options) {
    SearchInfo info;
    info.nodes = 0ULL;
    info.leafnodes = 0ULL;
    info.start = clock();
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
        ss[i].killer1 = 0;
        ss[i].killer2 = 0;
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
                info.end =
                    info.start +
                    ((double)(options.btime / options.movestogo) / 1000.0) *
                        CLOCKS_PER_SEC;
            } else {
                info.end =
                    info.start +
                    ((double)(options.wtime / options.movestogo) / 1000.0) *
                        CLOCKS_PER_SEC;
            }
            break;
        case SearchType::Movetime:
            options.depth = std::numeric_limits<int>::max();
            options.nodes = std::numeric_limits<uint64_t>::max();
            info.end = info.start +
                       ((double)options.movetime / 1000.0) * CLOCKS_PER_SEC;
            break;
        case SearchType::Depth:
            options.nodes = std::numeric_limits<uint64_t>::max();
            info.end = std::numeric_limits<clock_t>::max();
            break;
        case SearchType::Nodes:
            options.depth = std::numeric_limits<int>::max();
            info.end = std::numeric_limits<clock_t>::max();
            break;
        case SearchType::Infinite:
            options.nodes = std::numeric_limits<uint64_t>::max();
            options.depth = std::numeric_limits<int>::max();
            info.end = std::numeric_limits<clock_t>::max();
            break;
        default:
            options.depth = std::numeric_limits<int>::max();
            options.nodes = std::numeric_limits<uint64_t>::max();
            info.end = info.start + 1.0 * CLOCKS_PER_SEC;
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

        // If we ran out of time or were asked to stop, discard the result
        if (d > 1 && (clock() >= info.end || *info.stop == true)) {
            break;
        }

        memcpy(pv.moves, npv.moves, npv.length * sizeof(Move));
        pv.length = npv.length;

        assert(pv.length > 0);
        assert(pv.legal(pos) == true);

        double time_taken = (double)(clock() - info.start) / CLOCKS_PER_SEC;

        std::cout << "info";
        std::cout << " depth " << d;

        if (score > INF - MAX_DEPTH) {
            std::cout << " score mate " << (INF - score + 1) / 2;
        } else if (score < -INF + MAX_DEPTH) {
            std::cout << " score mate " << (-INF - score - 1) / 2;
        } else {
            std::cout << " score cp " << score;
        }

        std::cout << " nodes " << info.nodes;
        if (time_taken > 0) {
            std::cout << " nps " << int(info.nodes / time_taken);
        }
        std::cout << " time " << (int)(1000 * time_taken);

        if (pv.length > 0) {
            std::cout << " pv " + pv.string(pos.flipped);
        }

        std::cout << std::endl;
        /*
                // Try and predict if we have enough time for the next iteration
                if(info.start + 4*time_taken*CLOCKS_PER_SEC > info.end)
                {
                    break;
                }
        */
    }

#ifndef NDEBUG
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
        std::cout << "bestmove " << move_uci(pv.moves[0], pos.flipped)
                  << std::endl;
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

        assert(best_move == NO_MOVE);

        if (best_move != NO_MOVE) {
            std::cout << "bestmove " << move_uci(best_move, pos.flipped)
                      << std::endl;
        } else {
            std::cout << "bestmove 0000" << std::endl;
        }
    }
}

}  // namespace player
