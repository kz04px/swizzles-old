#ifndef KING_SAFETY_HPP
#define KING_SAFETY_HPP

#include "position.hpp"
#include "movegen.hpp"
#include "other.hpp"

int safety(const Position &pos)
{
    int sq = lsbll(pos.colour[Colour::US] & pos.pieces[PieceType::KING]);
    uint64_t surrounding = magic_moves_king(sq);
    return 5*popcountll(surrounding & pos.colour[Colour::US]);
}

#endif
