#include <cstdint>
#include <cassert>
#include "position.hpp"
#include "bitboards.hpp"
#include "types.hpp"
#include "move.hpp"
#include "attacks.hpp"
#include "movegen.hpp"
#include "other.hpp"

int movegen(const Position &pos, Move *movelist)
{
    assert(movelist != NULL);

    int num_moves = 0;
    uint64_t copy = 0ULL;
    const uint64_t allowed = ~pos.colour[Colour::US];

    // Pawns -- Singles
    copy = ((pos.pieces[PieceType::PAWN] & pos.colour[US]) << 8) & ~(pos.colour[US] | pos.colour[THEM]);
    while(copy)
    {
        Square to = Square(lsbll(copy));
        Square from = Square(to - 8);

        if(Square::A8 <= to && to <= Square::H8)
        {
            movelist[num_moves+0] = move(from, to, QUEEN_PROMO,  PieceType::PAWN);
            movelist[num_moves+1] = move(from, to, ROOK_PROMO,   PieceType::PAWN);
            movelist[num_moves+2] = move(from, to, BISHOP_PROMO, PieceType::PAWN);
            movelist[num_moves+3] = move(from, to, KNIGHT_PROMO, PieceType::PAWN);
            num_moves += 4;
        }
        else
        {
            movelist[num_moves] = move(from, to, QUIET, PieceType::PAWN);
            num_moves++;
        }

        copy &= copy-1;
    }

    // Pawns -- Doubles
    copy = (pos.pieces[PieceType::PAWN] & pos.colour[US] & U64_RANK_2); // Get all our pawns on the second rank
    copy = copy << 8; // Move them to the third rank
    copy = copy & ~(pos.colour[US] | pos.colour[THEM]); // Remove those that are blocked
    copy = copy << 8; // Shift them to the fourth rank
    copy = copy & ~(pos.colour[US] | pos.colour[THEM]); // Remove those that are blocked
    while(copy)
    {
        Square to = Square(lsbll(copy));
        Square from = Square(to - 16);
        movelist[num_moves] = move(from, to, DOUBLE, PieceType::PAWN);
        num_moves++;
        copy &= copy-1;
    }

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
            if(captured == PieceType::NONE)
            {
                movelist[num_moves] = move(from, to, QUIET, PieceType::KNIGHT);
            }
            else
            {
                movelist[num_moves] = move(from, to, CAPTURE, PieceType::KNIGHT, captured);
            }
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
            if(captured == PieceType::NONE)
            {
                movelist[num_moves] = move(from, to, QUIET, PieceType::BISHOP);
            }
            else
            {
                movelist[num_moves] = move(from, to, CAPTURE, PieceType::BISHOP, captured);
            }
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
            if(captured == PieceType::NONE)
            {
                movelist[num_moves] = move(from, to, QUIET, PieceType::ROOK);
            }
            else
            {
                movelist[num_moves] = move(from, to, CAPTURE, PieceType::ROOK, captured);
            }
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
            if(captured == PieceType::NONE)
            {
                movelist[num_moves] = move(from, to, QUIET, PieceType::QUEEN);
            }
            else
            {
                movelist[num_moves] = move(from, to, CAPTURE, PieceType::QUEEN, captured);
            }
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
            if(captured == PieceType::NONE)
            {
                movelist[num_moves] = move(from, to, QUIET, PieceType::QUEEN);
            }
            else
            {
                movelist[num_moves] = move(from, to, CAPTURE, PieceType::QUEEN, captured);
            }
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
        if(captured == PieceType::NONE)
        {
            movelist[num_moves] = move(from, to, QUIET, PieceType::KING);
        }
        else
        {
            movelist[num_moves] = move(from, to, CAPTURE, PieceType::KING, captured);
        }
        num_moves++;
        moves &= moves-1;
    }

    // Castling -- King side
    if(pos.castling[usKSC] == true &&
       piece_get(pos, Square::F1) == PieceType::NONE &&
       piece_get(pos, Square::G1) == PieceType::NONE &&
       attacked(pos, Square::E1, Colour::THEM) == false &&
       attacked(pos, Square::F1, Colour::THEM) == false &&
       attacked(pos, Square::G1, Colour::THEM) == false)
    {
        assert(pos.colour[US] & pos.pieces[PieceType::ROOK] & U64_FILE_H & U64_RANK_1);
        assert(pos.colour[US] & pos.pieces[PieceType::KING] & U64_FILE_E & U64_RANK_1);

        movelist[num_moves] = move(Square::E1, Square::G1, KSC, PieceType::KING);
        num_moves++;
    }

    // Castling -- Queen side
    if(pos.castling[usQSC] == true &&
       piece_get(pos, Square::B1) == PieceType::NONE &&
       piece_get(pos, Square::C1) == PieceType::NONE &&
       piece_get(pos, Square::D1) == PieceType::NONE &&
       attacked(pos, Square::E1, Colour::THEM) == false &&
       attacked(pos, Square::D1, Colour::THEM) == false &&
       attacked(pos, Square::C1, Colour::THEM) == false)
    {
        assert(pos.colour[US] & pos.pieces[PieceType::ROOK] & U64_FILE_A & U64_RANK_1);
        assert(pos.colour[US] & pos.pieces[PieceType::KING] & U64_FILE_E & U64_RANK_1);

        movelist[num_moves] = move(Square::E1, Square::C1, QSC, PieceType::KING);
        num_moves++;
    }

    assert(num_moves >= 0);
    assert(num_moves <= 256);

    return num_moves;
}
