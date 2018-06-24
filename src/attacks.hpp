#ifndef ATTACKS_HPP
#define ATTACKS_HPP

#include "position.hpp"
#include "types.hpp"

uint64_t squares_attacked(const Position &pos, const Colour side);
uint64_t attackers(const Position &pos, const Square sq, const Colour side);
int num_attackers(const Position &pos, const Square sq, const Colour side);
bool attacked(const Position &pos, const Square sq, const Colour side);
bool check(const Position &pos, const Colour side);
uint64_t pawn_attacks(const Position &pos, const Colour side);
uint64_t knight_attacks(const Position &pos, const Colour side);
uint64_t bishop_attacks(const Position &pos, const Colour side);
uint64_t rook_attacks(const Position &pos, const Colour side);
uint64_t queen_attacks(const Position &pos, const Colour side);
uint64_t king_attacks(const Position &pos, const Colour side);

#endif
