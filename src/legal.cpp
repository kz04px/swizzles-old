#include "legal.hpp"
#include "movegen.hpp"

bool legal_move(const Position &pos, const Move &move)
{
    Move moves[256];
    int num_moves = movegen(pos, moves);

    for(int i = 0; i < num_moves; ++i)
    {
        if(move == moves[i])
        {
            return true;
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
            return true;
        }
    }

    return false;
}
