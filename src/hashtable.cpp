#include "hashtable.hpp"

int eval_to_tt(int eval, int ply)
{
    if(eval >  INF-MAX_DEPTH) {return eval + ply;}
    if(eval < -INF+MAX_DEPTH) {return eval - ply;}
    return eval;
}

int eval_from_tt(int eval, int ply)
{
    if(eval >  INF-MAX_DEPTH) {return eval - ply;}
    if(eval < -INF+MAX_DEPTH) {return eval + ply;}
    return eval;
}
