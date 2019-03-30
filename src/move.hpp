#ifndef MOVE_HPP
#define MOVE_HPP

#include <cstdint>
#include <string>
#include "types.hpp"

typedef uint32_t Move;

#define NO_MOVE 0

Move move(Square from, Square to, MoveType type, PieceType piece);
Move move(Square from,
          Square to,
          MoveType type,
          PieceType piece,
          PieceType captured);
Move move(Square from,
          Square to,
          MoveType type,
          PieceType piece,
          PieceType captured,
          PieceType promo);
Square move_to(const Move &m);
Square move_from(const Move &m);
PieceType move_piece(const Move &m);
MoveType move_type(const Move &m);
PieceType move_captured(const Move &m);
PieceType move_promo(const Move &m);
std::string move_uci(const Move &m, const bool flipped);
std::string move_details(const Move &m, const bool flipped);

#endif
