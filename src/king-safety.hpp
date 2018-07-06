#ifndef KING_SAFETY_HPP
#define KING_SAFETY_HPP

#include "position.hpp"
#include "movegen.hpp"
#include "other.hpp"

const int attacker_values[64] = {0, 0, 20, 40, 80, 90};

uint64_t shield(const int sq)
{
    uint64_t bb = 1ULL << sq;
    bb  = bb<<8;
    bb |= bb<<8;
    bb |= (bb<<1) & (~U64_FILE_A);
    bb |= (bb>>1) & (~U64_FILE_H);
    return bb;
}

int safety(const Position &pos)
{
    const uint64_t bb = pos.colour[Colour::US] & pos.pieces[PieceType::KING];
    const int sq = lsbll(bb);
    const uint64_t pawn_shield = shield(sq);

    int score = 0;

    // Pawn shield
    score += 15*popcountll(pawn_shield & pos.colour[Colour::US] & pos.pieces[PieceType::PAWN]);

    return score;
}

#endif
