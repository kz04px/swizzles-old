#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>
#include "types.hpp"

struct Position
{
    bool flipped;
    int halfmoves;
    int fullmoves;
    Square enpassant;
    bool castling[4];
    uint64_t colour[2];
    uint64_t pieces[6];
    int history_size;
    std::uint64_t history[128];
};

PieceType piece_get(const Position &pos, const Square sq);
bool operator == (const Position &a, const Position &b);
bool operator != (const Position &a, const Position &b);

#endif
