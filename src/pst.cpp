#include "pst.hpp"
#include "types.hpp"
#include "other.hpp"

int score_pst(const Position &pos, const int stage, const int piece)
{
    int score = 0;
    uint64_t copy = pos.colour[Colour::US] & pos.pieces[piece];
    while(copy)
    {
        int sq = lsbll(copy);

        score += PST[piece][stage][sq];

        copy &= copy -1;
    }
    return score;
}
