#include "qsearch.hpp"
#include "movegen.hpp"
#include "move.hpp"
#include "makemove.hpp"
#include "eval.hpp"
#include "types.hpp"
#include "attacks.hpp"
#include "see.hpp"
#include "sort.hpp"

int qsearch(const Position &pos, SearchInfo &info, SearchStack *ss, int alpha, int beta)
{
    if(ss->ply >= 256)
    {
        return eval(pos);
    }

    if(*info.stop == true || clock() > info.end)
    {
        return 0;
    }

    int stand_pat = eval(pos);

    if(stand_pat >= beta)
    {
        return beta;
    }

    if(alpha < stand_pat)
    {
        alpha = stand_pat;
    }

    Move moves[256];
    int num_moves = movegen_captures(pos, moves);
    if(num_moves == 0) {return alpha;}
    //int scores[256];
    //sort_see(pos, moves, scores, num_moves);

    for(int i = 0; i < num_moves; ++i)
    {
        //if(scores[i] < -50) {break;}
        if(see_capture(pos, moves[i]) < -50) {continue;}

        Position npos = pos;
        make_move(npos, moves[i]);
        if(check(npos, Colour::THEM) == true) {continue;}

        int score = -qsearch(npos, info, ss+1, -beta, -alpha);

        if(score >= beta)
        {
            return beta;
        }

        if(score > alpha)
        {
            alpha = score;
        }
    }

    return alpha;
}
