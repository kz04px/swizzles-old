#include <cstdint>
#include <cassert>
#include "position.hpp"
#include "bitboards.hpp"
#include "types.hpp"
#include "move.hpp"
#include "attacks.hpp"
#include "movegen.hpp"
#include "other.hpp"

int movegen_captures(const Position &pos, Move *movelist)
{
    assert(movelist != NULL);

    int num_moves = 0;
    uint64_t copy = 0ULL;
    const uint64_t allowed = pos.colour[THEM];

    // Pawns -- Captures left
    copy = ((pos.pieces[PieceType::PAWN] & pos.colour[US] & ~U64_FILE_A) << 7) & pos.colour[THEM];
    while(copy)
    {
        Square to = Square(lsbll(copy));
        Square from = Square(to - 7);
        PieceType captured = piece_get(pos, to);

        if(Square::A8 <= to && to <= Square::H8)
        {
            movelist[num_moves+0] = move(from, to, QUEEN_PROMO_CAPTURE,  PieceType::PAWN, captured);
            movelist[num_moves+1] = move(from, to, ROOK_PROMO_CAPTURE,   PieceType::PAWN, captured);
            movelist[num_moves+2] = move(from, to, BISHOP_PROMO_CAPTURE, PieceType::PAWN, captured);
            movelist[num_moves+3] = move(from, to, KNIGHT_PROMO_CAPTURE, PieceType::PAWN, captured);
            num_moves += 4;
        }
        else
        {
            movelist[num_moves] = move(from, to, CAPTURE, PieceType::PAWN, captured);
            num_moves++;
        }
        copy &= copy-1;
    }

    // Pawns -- Captures right
    copy = ((pos.pieces[PieceType::PAWN] & pos.colour[US] & ~U64_FILE_H) << 9) & pos.colour[THEM];
    while(copy)
    {
        Square to = Square(lsbll(copy));
        Square from = Square(to - 9);
        PieceType captured = piece_get(pos, to);

        if(Square::A8 <= to && to <= Square::H8)
        {
            movelist[num_moves+0] = move(from, to, QUEEN_PROMO_CAPTURE,  PieceType::PAWN, captured);
            movelist[num_moves+1] = move(from, to, ROOK_PROMO_CAPTURE,   PieceType::PAWN, captured);
            movelist[num_moves+2] = move(from, to, BISHOP_PROMO_CAPTURE, PieceType::PAWN, captured);
            movelist[num_moves+3] = move(from, to, KNIGHT_PROMO_CAPTURE, PieceType::PAWN, captured);
            num_moves += 4;
        }
        else
        {
            movelist[num_moves] = move(from, to, CAPTURE, PieceType::PAWN, captured);
            num_moves++;
        }
        copy &= copy-1;
    }

    // Enpassant
    if(pos.enpassant != Square::OFFSQ)
    {
        uint64_t moves = magic_moves_pawn(THEM, pos.enpassant) & pos.pieces[PieceType::PAWN] & pos.colour[US];
        while(moves)
        {
            Square from = Square(lsbll(moves));
            movelist[num_moves] = move(from, pos.enpassant, ENPASSANT, PieceType::PAWN, PieceType::PAWN);
            assert(move_captured(movelist[num_moves]) == PieceType::PAWN);
            num_moves++;
            moves &= moves-1;
        }
    }

    // Knights
    copy = pos.pieces[PieceType::KNIGHT] & pos.colour[US];
    while(copy)
    {
        Square from = Square(lsbll(copy));
        uint64_t moves = magic_moves_knight(from) & allowed;

        while(moves)
        {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] = move(from, to, CAPTURE, PieceType::KNIGHT, captured);
            num_moves++;
            moves &= moves-1;
        }

        copy &= copy-1;
    }

    // Bishops
    copy = pos.pieces[PieceType::BISHOP] & pos.colour[US];
    while(copy)
    {
        Square from = Square(lsbll(copy));
        uint64_t moves = magic_moves_bishop((pos.colour[US] | pos.colour[THEM]), from) & allowed;

        while(moves)
        {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] = move(from, to, CAPTURE, PieceType::BISHOP, captured);
            num_moves++;
            moves &= moves-1;
        }

        copy &= copy-1;
    }

    // Rook
    copy = pos.pieces[PieceType::ROOK] & pos.colour[US];
    while(copy)
    {
        Square from = Square(lsbll(copy));
        uint64_t moves = magic_moves_rook((pos.colour[US] | pos.colour[THEM]), from) & allowed;

        while(moves)
        {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] = move(from, to, CAPTURE, PieceType::ROOK, captured);
            num_moves++;
            moves &= moves-1;
        }

        copy &= copy-1;
    }

    // Queens -- Bishop
    copy = pos.pieces[PieceType::QUEEN] & pos.colour[US];
    while(copy)
    {
        Square from = Square(lsbll(copy));
        uint64_t moves = magic_moves_bishop((pos.colour[US] | pos.colour[THEM]), from) & allowed;

        while(moves)
        {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] = move(from, to, CAPTURE, PieceType::QUEEN, captured);
            num_moves++;
            moves &= moves-1;
        }

        copy &= copy-1;
    }

    // Queens -- Rook
    copy = pos.pieces[PieceType::QUEEN] & pos.colour[US];
    while(copy)
    {
        Square from = Square(lsbll(copy));
        uint64_t moves = magic_moves_rook((pos.colour[US] | pos.colour[THEM]), from) & allowed;

        while(moves)
        {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] = move(from, to, CAPTURE, PieceType::QUEEN, captured);
            num_moves++;
            moves &= moves-1;
        }

        copy &= copy-1;
    }

    // King
    Square from = Square(lsbll(pos.pieces[PieceType::KING] & pos.colour[US]));
    uint64_t moves = magic_moves_king(from) & allowed;
    while(moves)
    {
        Square to = Square(lsbll(moves));
        PieceType captured = piece_get(pos, to);
        movelist[num_moves] = move(from, to, CAPTURE, PieceType::KING, captured);
        num_moves++;
        moves &= moves-1;
    }

    assert(num_moves >= 0);
    assert(num_moves <= 128);

#ifndef NDEBUG
    for(int n = 0; n < num_moves; ++n)
    {
        assert(move_captured(movelist[n]) != PieceType::NONE);
        assert(move_type(movelist[n]) == MoveType::CAPTURE ||
               move_type(movelist[n]) == MoveType::ENPASSANT ||
               move_type(movelist[n]) == MoveType::KNIGHT_PROMO_CAPTURE ||
               move_type(movelist[n]) == MoveType::BISHOP_PROMO_CAPTURE ||
               move_type(movelist[n]) == MoveType::ROOK_PROMO_CAPTURE ||
               move_type(movelist[n]) == MoveType::QUEEN_PROMO_CAPTURE
               );
    }
#endif

    return num_moves;
}
