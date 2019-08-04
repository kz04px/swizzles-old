#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>
#include "move.hpp"
#include "types.hpp"

struct Position {
    bool flipped;
    int halfmoves;
    int fullmoves;
    Square enpassant;
    bool castling[4];
    uint64_t colour[2];
    uint64_t pieces[6];
    uint64_t hash;
    int history_size;
    std::uint64_t history[128];
};

PieceType piece_get(const Position &pos, const Square sq);
int repetitions(const Position &pos);
bool is_fifty_moves(const Position &pos);
int is_endgame(const Position &pos);
bool operator==(const Position &a, const Position &b);
bool operator!=(const Position &a, const Position &b);
bool legal_move(const Position &pos, const Move &move);
bool pseudolegal_move(const Position &pos, const Move &move);
bool legal_move(const Position &pos, const std::string &move);
bool legal_position(const Position &pos);
void flip(Position &pos);

#endif
