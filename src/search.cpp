#include <iostream>
#include <cstdint>
#include <climits>
#include <cstring>
#include <cassert>
#include "search.hpp"
#include "alphabeta.hpp"
#include "searchinfo.hpp"
#include "searchstack.hpp"
#include "pv.hpp"
#include "makemove.hpp"
#include "legal.hpp"
#include "movegen.hpp"
#include "display.hpp"
#include "attacks.hpp"

void search(const Position &pos, Hashtable &tt, bool &stop, int depth, int movetime, int nodes, bool infinite, int wtime, int btime, int winc, int binc, int movestogo)
{
    SearchInfo info;
    info.nodes = 0ULL;
    info.leafnodes = 0ULL;
    info.start = clock();
    info.stop = &stop;
    info.tt = &tt;
#ifndef NDEBUG
    for(int i = 0; i < 256; ++i)
    {
        info.cutoffs[i] = 0ULL;
    }
#endif


    SearchStack ss[MAX_DEPTH+1];
    for(int i = 0; i < MAX_DEPTH+1; ++i)
    {
        ss[i].ply = i;
        ss[i].nullmove = true;
        ss[i].killer1 = 0;
        ss[i].killer2 = 0;
    }


    if(infinite)
    {
        info.end = INT_MAX;
        depth = INT_MAX;
    }
    else if(movetime >= 0)
    {
        info.end = info.start + ((double)movetime/1000.0)*CLOCKS_PER_SEC;
        depth = INT_MAX;
    }
    else if(depth >= 0)
    {
        info.end = INT_MAX;
    }
    else if(nodes >= 0)
    {
        info.end = INT_MAX;
    }
    else if(wtime >= 0 && btime >= 0)
    {
        if(pos.flipped)
        {
            info.end = info.start + ((double)(btime/30)/1000.0)*CLOCKS_PER_SEC;
            depth = INT_MAX;
        }
        else
        {
            info.end = info.start + ((double)(wtime/30)/1000.0)*CLOCKS_PER_SEC;
            depth = INT_MAX;
        }
    }
    else
    {
        info.end = info.start + 1.0*CLOCKS_PER_SEC;
        depth = INT_MAX;
    }


    PV pv;
    for(int d = 1; d <= depth && d < MAX_DEPTH; ++d)
    {
        PV npv;

        int score = 0;
        if(depth < 3)
        {
            score = alphabeta(pos, info, ss, npv, -INF, INF, d);
        }
        else
        {
            for(auto &b : {50, 200, INF})
            {
                score = alphabeta(pos, info, ss, npv, -b, b, d);

                if(-b < score && score < b)
                {
                    break;
                }
            }
        }

        // If we ran out of time or were asked to stop, discard the result
        if(d > 1 && (clock() >= info.end || *info.stop == true))
        {
            break;
        }

        memcpy(pv.moves, npv.moves, npv.length * sizeof(Move));
        pv.length = npv.length;

        assert(pv.length > 0);

        double time_taken = (double)(clock() - info.start)/CLOCKS_PER_SEC;

        std::cout << "info"
                  << " depth " << d
                  << " nodes " << info.nodes
                  << " time " << (int)(1000*time_taken);

        if(abs(score) < INF - MAX_DEPTH)
        {
            std::cout << " score cp " << score;
        }
        else
        {
            std::cout << " score mate " << score;
        }

        if(pv.length > 0)
        {
            std::cout << " pv";
            for(int n = 0; n < pv.length; ++n)
            {
                std::cout << " " << move_uci(pv.moves[n], pos.flipped ^ (n % 2));
            }
            assert(pv.legal(pos) == true);
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
    for(int i = 0; i < 256; ++i)
    {
        sum += info.cutoffs[i];
    }
    for(int i = 0; i < 8; ++i)
    {
        std::cout << "info string move " << i+1 << " beta cutoffs " << info.cutoffs[i] << " ";
        if(sum > 0ULL)
        {
            std::cout << "(" << 100.0*(double)info.cutoffs[i]/sum << "%)";
        }
        else
        {
            std::cout << "-";
        }
        std::cout << std::endl;
    }
#endif


    if(pv.length > 0)
    {
        std::cout << "bestmove " << move_uci(pv.moves[0], pos.flipped) << std::endl;
    }
    else
    {
#ifndef NDEBUG
        Move moves[256];
        int num_moves = movegen(pos, moves);

        bool legal_moves = false;
        for(int i = 0; i < num_moves; ++i)
        {
            Position npos = pos;
            make_move(npos, moves[i]);
            if(check(npos, Colour::THEM) == true) {continue;}

            legal_moves = true;
            break;
        }

        assert(legal_moves == false);
#endif

        std::cout << "bestmove 0000" << std::endl;
    }
}
