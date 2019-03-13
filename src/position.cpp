#include <cstdint>
#include "position.hpp"
#include "types.hpp"

PieceType piece_get(const Position &pos, const Square sq)
{
    uint64_t bb = 1ULL << sq;

    for(int i = PieceType::PAWN; i <= PieceType::KING; ++i)
    {
        if(pos.pieces[i] & bb)
        {
            return PieceType(i);
        }
    }

    return PieceType::NONE;
}

bool operator == (const Position &a, const Position &b)
{
    // Flipped
    if(a.flipped != b.flipped) {return false;};

    // Pieces
    if(a.pieces[PieceType::PAWN]   != b.pieces[PieceType::PAWN])   {return false;};
    if(a.pieces[PieceType::KNIGHT] != b.pieces[PieceType::KNIGHT]) {return false;};
    if(a.pieces[PieceType::BISHOP] != b.pieces[PieceType::BISHOP]) {return false;};
    if(a.pieces[PieceType::ROOK]   != b.pieces[PieceType::ROOK])   {return false;};
    if(a.pieces[PieceType::QUEEN]  != b.pieces[PieceType::QUEEN])  {return false;};
    if(a.pieces[PieceType::KING]   != b.pieces[PieceType::KING])   {return false;};

    // Colours
    if(a.colour[Colour::US]   != b.colour[Colour::US])   {return false;};
    if(a.colour[Colour::THEM] != b.colour[Colour::THEM]) {return false;};

    // Castling
    if(a.castling[usKSC]   != b.castling[usKSC])   {return false;};
    if(a.castling[usQSC]   != b.castling[usQSC])   {return false;};
    if(a.castling[themKSC] != b.castling[themKSC]) {return false;};
    if(a.castling[themQSC] != b.castling[themQSC]) {return false;};

    // Enpassant
    if(a.enpassant != b.enpassant) {return false;};

    // Move counts
    if(a.halfmoves != b.halfmoves) {return false;};
    if(a.fullmoves != b.fullmoves) {return false;};

    return true;
}

bool operator != (const Position &a, const Position &b)
{
    return !(a == b);
}

int repetitions(const Position &pos)
{
    int count = 0;
    for(int i = pos.history_size-3; i >= 0; --i)
    {
        if(pos.history[i] == pos.history[pos.history_size-1])
        {
            count++;
        }
    }
    return count;
}

bool is_fifty_moves(const Position &pos)
{
    return pos.halfmoves >= 100;
}

int is_endgame(const Position &pos)
{
    return popcountll(pos.colour[pos.flipped] & (pos.pieces[PieceType::KNIGHT] | pos.pieces[PieceType::BISHOP] | pos.pieces[PieceType::ROOK] | pos.pieces[PieceType::QUEEN])) <= 2;
}
