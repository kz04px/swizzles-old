#include <cassert>
#include "rollout.hpp"
#include "movegen.hpp"
#include "makemove.hpp"
#include "attacks.hpp"
#include "valid.hpp"

int repetitions2(const Position &pos)
{
    int count = 0;
    for(int i = pos.history_size-3; i >= 0 ; --i)
    {
        if(pos.history[i] == pos.history[pos.history_size-1])
        {
            count++;
        }
    }
    return count;
}

bool is_fifty_moves2(const Position &pos)
{
    return pos.halfmoves >= 100;
}

float rollout(const Position &pos, const int length)
{
    assert(valid(pos) == true);
    assert(length > 0);

    Position main_pos = pos;
    Position npos;
    Move moves[256];

    float r = 0.5;
    int n = 0;
    while(n < length)
    {
        if(is_fifty_moves2(main_pos) || repetitions2(main_pos) == 2) //  || (repetitions(pos) == 1 && ss->ply > 2)
        {
            r = 0.5;
            break;
        }

        int num_moves = movegen(main_pos, moves);
        Move best_move = NO_MOVE;
        const bool in_check = check(main_pos, Colour::US);

        while(num_moves > 0)
        {
            npos = main_pos;
            int i = rand()%num_moves;

            make_move(npos, moves[i]);
            if(check(npos, Colour::THEM) == true)
            {
                assert(num_moves > 0);
                moves[i] = moves[num_moves-1];
                num_moves--;
                continue;
            }

            best_move = moves[i];
            break;
        }

        if(best_move == NO_MOVE)
        {
            if(in_check == true)
            {
                r = 0.0;
                break;
            }
            else
            {
                r = 0.5;
                break;
            }
        }

        make_move(main_pos, best_move);
        assert(check(main_pos, Colour::THEM) == false);

        n++;
    }

    assert(r >= 0.0);
    assert(r <= 1.0);

    if(main_pos.flipped == pos.flipped)
    {
        return r;
    }
    else
    {
        return 1.0 - r;
    }
}
