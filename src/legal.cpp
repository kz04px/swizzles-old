#include "legal.hpp"
#include "movegen.hpp"
#include "makemove.hpp"
#include "attacks.hpp"

bool legal_move(const Position &pos, const Move &move)
{
    Move moves[256];
    int num_moves = movegen(pos, moves);

    for(int i = 0; i < num_moves; ++i)
    {
        if(move == moves[i])
        {
            Position npos = pos;
            make_move(npos, moves[i]);
            return !check(npos, Colour::THEM);
        }
    }

    return false;
}

bool legal_move(const Position &pos, const std::string &move)
{
    Move moves[256];
    int num_moves = movegen(pos, moves);

    for(int i = 0; i < num_moves; ++i)
    {
        if(move == move_uci(moves[i], pos.flipped))
        {
            Position npos = pos;
            make_move(npos, moves[i]);
            return !check(npos, Colour::THEM);
        }
    }

    return false;
}
