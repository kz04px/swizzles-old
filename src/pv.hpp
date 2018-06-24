#ifndef PV_HPP
#define PV_HPP

#include <cassert>
#include "move.hpp"
#include "legal.hpp"
#include "makemove.hpp"

struct PV
{
    PV() : length(0)
    {
    }

    bool legal(const Position &pos) const
    {
        Position npos = pos;
        for(int i = 0; i < length; ++i)
        {
            if(legal_move(npos, moves[i]) == false)
            {
                return false;
            }

            make_move(npos, moves[i]);
        }
        return true;
    }

    void add(const Move m)
    {
        moves[length] = m;
        length++;
        assert(length < 64);
    }

    int length;
    Move moves[64];
};

#endif
