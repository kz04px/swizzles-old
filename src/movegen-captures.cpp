#include "assert.hpp"
#include <cstdint>
#include "attacks.hpp"
#include "bitboards.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "other.hpp"
#include "position.hpp"
#include "types.hpp"

int movegen_captures(const Position &pos, Move *movelist) {
    UCI_ASSERT(movelist);

    int num_moves = 0;
    uint64_t copy = 0ULL;
    const uint64_t allowed = pos.colour[THEM];

    // Pawns -- Captures left
    copy = ((pos.pieces[PieceType::PAWN] & pos.colour[US] & ~U64_FILE_A) << 7) &
           pos.colour[THEM];
    while (copy) {
        Square to = Square(lsbll(copy));
        Square from = Square(to - 7);
        PieceType captured = piece_get(pos, to);

        if (Square::A8 <= to && to <= Square::H8) {
            movelist[num_moves + 0] = Move(from,
                                           to,
                                           PROMO_CAPTURE,
                                           PieceType::PAWN,
                                           captured,
                                           PieceType::QUEEN);
            movelist[num_moves + 1] = Move(from,
                                           to,
                                           PROMO_CAPTURE,
                                           PieceType::PAWN,
                                           captured,
                                           PieceType::ROOK);
            movelist[num_moves + 2] = Move(from,
                                           to,
                                           PROMO_CAPTURE,
                                           PieceType::PAWN,
                                           captured,
                                           PieceType::BISHOP);
            movelist[num_moves + 3] = Move(from,
                                           to,
                                           PROMO_CAPTURE,
                                           PieceType::PAWN,
                                           captured,
                                           PieceType::KNIGHT);
            num_moves += 4;
        } else {
            movelist[num_moves] =
                Move(from, to, CAPTURE, PieceType::PAWN, captured);
            num_moves++;
        }
        copy &= copy - 1;
    }

    // Pawns -- Captures right
    copy = ((pos.pieces[PieceType::PAWN] & pos.colour[US] & ~U64_FILE_H) << 9) &
           pos.colour[THEM];
    while (copy) {
        Square to = Square(lsbll(copy));
        Square from = Square(to - 9);
        PieceType captured = piece_get(pos, to);

        if (Square::A8 <= to && to <= Square::H8) {
            movelist[num_moves + 0] = Move(from,
                                           to,
                                           PROMO_CAPTURE,
                                           PieceType::PAWN,
                                           captured,
                                           PieceType::QUEEN);
            movelist[num_moves + 1] = Move(from,
                                           to,
                                           PROMO_CAPTURE,
                                           PieceType::PAWN,
                                           captured,
                                           PieceType::ROOK);
            movelist[num_moves + 2] = Move(from,
                                           to,
                                           PROMO_CAPTURE,
                                           PieceType::PAWN,
                                           captured,
                                           PieceType::BISHOP);
            movelist[num_moves + 3] = Move(from,
                                           to,
                                           PROMO_CAPTURE,
                                           PieceType::PAWN,
                                           captured,
                                           PieceType::KNIGHT);
            num_moves += 4;
        } else {
            movelist[num_moves] =
                Move(from, to, CAPTURE, PieceType::PAWN, captured);
            num_moves++;
        }
        copy &= copy - 1;
    }

    // Enpassant
    if (pos.enpassant != Square::A1) {
        uint64_t moves = magic_moves_pawn(THEM, pos.enpassant) &
                         pos.pieces[PieceType::PAWN] & pos.colour[US];
        while (moves) {
            Square from = Square(lsbll(moves));
            movelist[num_moves] = Move(from,
                                       pos.enpassant,
                                       ENPASSANT,
                                       PieceType::PAWN,
                                       PieceType::PAWN);
            UCI_ASSERT(movelist[num_moves].captured() == PieceType::PAWN);
            num_moves++;
            moves &= moves - 1;
        }
    }

    // Knights
    copy = pos.pieces[PieceType::KNIGHT] & pos.colour[US];
    while (copy) {
        Square from = Square(lsbll(copy));
        uint64_t moves = magic_moves_knight(from) & allowed;

        while (moves) {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] =
                Move(from, to, CAPTURE, PieceType::KNIGHT, captured);
            num_moves++;
            moves &= moves - 1;
        }

        copy &= copy - 1;
    }

    // Bishops
    copy = pos.pieces[PieceType::BISHOP] & pos.colour[US];
    while (copy) {
        Square from = Square(lsbll(copy));
        uint64_t moves =
            magic_moves_bishop((pos.colour[US] | pos.colour[THEM]), from) &
            allowed;

        while (moves) {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] =
                Move(from, to, CAPTURE, PieceType::BISHOP, captured);
            num_moves++;
            moves &= moves - 1;
        }

        copy &= copy - 1;
    }

    // Rook
    copy = pos.pieces[PieceType::ROOK] & pos.colour[US];
    while (copy) {
        Square from = Square(lsbll(copy));
        uint64_t moves =
            magic_moves_rook((pos.colour[US] | pos.colour[THEM]), from) &
            allowed;

        while (moves) {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] =
                Move(from, to, CAPTURE, PieceType::ROOK, captured);
            num_moves++;
            moves &= moves - 1;
        }

        copy &= copy - 1;
    }

    // Queens -- Bishop
    copy = pos.pieces[PieceType::QUEEN] & pos.colour[US];
    while (copy) {
        Square from = Square(lsbll(copy));
        uint64_t moves =
            magic_moves_bishop((pos.colour[US] | pos.colour[THEM]), from) &
            allowed;

        while (moves) {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] =
                Move(from, to, CAPTURE, PieceType::QUEEN, captured);
            num_moves++;
            moves &= moves - 1;
        }

        copy &= copy - 1;
    }

    // Queens -- Rook
    copy = pos.pieces[PieceType::QUEEN] & pos.colour[US];
    while (copy) {
        Square from = Square(lsbll(copy));
        uint64_t moves =
            magic_moves_rook((pos.colour[US] | pos.colour[THEM]), from) &
            allowed;

        while (moves) {
            Square to = Square(lsbll(moves));
            PieceType captured = piece_get(pos, to);
            movelist[num_moves] =
                Move(from, to, CAPTURE, PieceType::QUEEN, captured);
            num_moves++;
            moves &= moves - 1;
        }

        copy &= copy - 1;
    }

    // King
    Square from = Square(lsbll(pos.pieces[PieceType::KING] & pos.colour[US]));
    uint64_t moves = magic_moves_king(from) & allowed;
    while (moves) {
        Square to = Square(lsbll(moves));
        PieceType captured = piece_get(pos, to);
        movelist[num_moves] =
            Move(from, to, CAPTURE, PieceType::KING, captured);
        num_moves++;
        moves &= moves - 1;
    }

    UCI_ASSERT(num_moves >= 0);
    UCI_ASSERT(num_moves <= 128);

#ifndef NDEBUG
    for (int n = 0; n < num_moves; ++n) {
        UCI_ASSERT(pseudolegal_move(pos, movelist[n]) == true);
        UCI_ASSERT(movelist[n].captured() != PieceType::NONE);
        UCI_ASSERT(movelist[n].type() == MoveType::CAPTURE ||
               movelist[n].type() == MoveType::ENPASSANT ||
               movelist[n].type() == MoveType::PROMO_CAPTURE);
    }
#endif

    return num_moves;
}
