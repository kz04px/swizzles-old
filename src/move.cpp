#include <cassert>
#include "move.hpp"
#include "position.hpp"
#include "types.hpp"

Move move(Square from, Square to, MoveType type, PieceType piece)
{
    Move m;

    // To
    m = to & 0x3F;

    // From
    m |= (from & 0x3F) << 6;

    // Type
    m |= (type & 0xF) << 12;

    // Piece
    m |= (piece & 0x7) << 16;

    // Captured
    m |= (PieceType::NONE & 0x7) << 19;

    assert(move_to(m) == to);
    assert(move_from(m) == from);
    assert(move_type(m) == type);
    assert(move_piece(m) == piece);
    assert(move_captured(m) == PieceType::NONE);

    return m;
}

Move move(Square from, Square to, MoveType type, PieceType piece, PieceType captured)
{
    Move m;

    // To
    m = to & 0x3F;

    // From
    m |= (from & 0x3F) << 6;

    // Type
    m |= (type & 0xF) << 12;

    // Piece
    m |= (piece & 0x7) << 16;

    // Captured
    m |= (captured & 0x7) << 19;

    assert(move_to(m) == to);
    assert(move_from(m) == from);
    assert(move_type(m) == type);
    assert(move_piece(m) == piece);
    assert(move_captured(m) == captured);

    return m;
}

Square move_to(const Move &m)
{
    return Square(m & 0x3F);
}

Square move_from(const Move &m)
{
    return Square((m >> 6) & 0x3F);
}

MoveType move_type(const Move &m)
{
    return MoveType((m >> 12) & 0xF);
}

PieceType move_piece(const Move &m)
{
    return PieceType((m >> 16) & 0x7);
}

PieceType move_captured(const Move &m)
{
    return PieceType((m >> 19) & 0x7);
}

std::string move_uci(const Move &m, const bool flipped)
{
    Square from = move_from(m);
    Square to = move_to(m);
    MoveType type = move_type(m);

    if(flipped == true)
    {
        from = Square(from ^ 56);
        to = Square(to ^ 56);
    }

    std::string ans = SquareString[from] + SquareString[to];

    if(type == MoveType::QUEEN_PROMO || type == MoveType::QUEEN_PROMO_CAPTURE)
    {
        ans += "q";
    }
    else if(type == MoveType::ROOK_PROMO || type == MoveType::ROOK_PROMO_CAPTURE)
    {
        ans += "r";
    }
    else if(type == MoveType::BISHOP_PROMO || type == MoveType::BISHOP_PROMO_CAPTURE)
    {
        ans += "b";
    }
    else if(type == MoveType::KNIGHT_PROMO || type == MoveType::KNIGHT_PROMO_CAPTURE)
    {
        ans += "n";
    }

    return ans;
}

std::string move_details(const Move &m, const bool flipped)
{
    return move_uci(m, flipped) +
           "  (piece "    + std::to_string(move_piece(m))    + ")" +
           "  (type "     + std::to_string(move_type(m))     + ")" +
           "  (captured " + std::to_string(move_captured(m)) + ")";
}
