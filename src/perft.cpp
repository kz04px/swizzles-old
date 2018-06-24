#include <iostream>
#include <cstdint>
#include "perft.hpp"
#include "move.hpp"
#include "types.hpp"
#include "attacks.hpp"
#include "movegen.hpp"
#include "makemove.hpp"

uint64_t perft(const Position &pos, const int depth)
{
    if(depth == 0)
    {
        return 1ULL;
    }

    uint64_t nodes = 0;
    Move moves[256];
    int num_moves = movegen(pos, moves);

    for(int i = 0; i < num_moves; ++i)
    {
        Position npos = pos;
        make_move(npos, moves[i]);
        if(check(npos, Colour::THEM) == true) {continue;}
        nodes += perft(npos, depth-1);
    }

    return nodes;
}
