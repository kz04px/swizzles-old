#include "assert.hpp"
#include <cstdint>
#include "attacks.hpp"
#include "bitboards.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "other.hpp"
#include "position.hpp"
#include "types.hpp"

int movegen_noncaptures(const Position &pos, Move *movelist) {
    UCI_ASSERT(movelist);

    int num_moves = 0;
    uint64_t copy = 0ULL;
    const uint64_t allowed =
        ~(pos.colour[Colour::US] | pos.colour[Colour::THEM]);

    // Pawns -- Singles
    copy = ((pos.pieces[PieceType::PAWN] & pos.colour[US]) << 8) & allowed;
    while (copy) {
        Square to = Square(lsbll(copy));
        Square from = Square(to - 8);

        if (Square::A8 <= to && to <= Square::H8) {
            movelist[num_moves + 0] = Move(from,
                                           to,
                                           PROMO,
                                           PieceType::PAWN,
                                           PieceType::NONE,
                                           PieceType::QUEEN);
            movelist[num_moves + 1] = Move(from,
                                           to,
                                           PROMO,
                                           PieceType::PAWN,
                                           PieceType::NONE,
                                           PieceType::ROOK);
            movelist[num_moves + 2] = Move(from,
                                           to,
                                           PROMO,
                                           PieceType::PAWN,
                                           PieceType::NONE,
                                           PieceType::BISHOP);
            movelist[num_moves + 3] = Move(from,
                                           to,
                                           PROMO,
                                           PieceType::PAWN,
                                           PieceType::NONE,
                                           PieceType::KNIGHT);
            num_moves += 4;
        } else {
            movelist[num_moves] = Move(from, to, QUIET, PieceType::PAWN);
            num_moves++;
        }

        copy &= copy - 1;
    }

    // Pawns -- Doubles
    copy = (pos.pieces[PieceType::PAWN] & pos.colour[US] &
            U64_RANK_2);  // Get all our pawns on the second rank
    copy = copy << 8;     // Move them to the third rank
    copy = copy & ~(pos.colour[US] |
                    pos.colour[THEM]);  // Remove those that are blocked
    copy = copy << 8;                   // Shift them to the fourth rank
    copy = copy & ~(pos.colour[US] |
                    pos.colour[THEM]);  // Remove those that are blocked
    while (copy) {
        Square to = Square(lsbll(copy));
        Square from = Square(to - 16);
        movelist[num_moves] = Move(from, to, DOUBLE, PieceType::PAWN);
        num_moves++;
        copy &= copy - 1;
    }

    // Knights
    copy = pos.pieces[PieceType::KNIGHT] & pos.colour[US];
    while (copy) {
        Square from = Square(lsbll(copy));
        uint64_t moves = magic_moves_knight(from) & allowed;

        while (moves) {
            Square to = Square(lsbll(moves));
            movelist[num_moves] = Move(from, to, QUIET, PieceType::KNIGHT);
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
            movelist[num_moves] = Move(from, to, QUIET, PieceType::BISHOP);
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
            movelist[num_moves] = Move(from, to, QUIET, PieceType::ROOK);
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
            movelist[num_moves] = Move(from, to, QUIET, PieceType::QUEEN);
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
            movelist[num_moves] = Move(from, to, QUIET, PieceType::QUEEN);
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
        movelist[num_moves] = Move(from, to, QUIET, PieceType::KING);
        num_moves++;
        moves &= moves - 1;
    }

    // Castling -- King side
    if (pos.castling[usKSC] == true &&
        piece_get(pos, Square::F1) == PieceType::NONE &&
        piece_get(pos, Square::G1) == PieceType::NONE &&
        attacked(pos, Square::E1, Colour::THEM) == false &&
        attacked(pos, Square::F1, Colour::THEM) == false &&
        attacked(pos, Square::G1, Colour::THEM) == false) {
        UCI_ASSERT(pos.colour[US] & pos.pieces[PieceType::ROOK] & U64_FILE_H &
               U64_RANK_1);
        UCI_ASSERT(pos.colour[US] & pos.pieces[PieceType::KING] & U64_FILE_E &
               U64_RANK_1);

        movelist[num_moves] =
            Move(Square::E1, Square::G1, KSC, PieceType::KING);
        num_moves++;
    }

    // Castling -- Queen side
    if (pos.castling[usQSC] == true &&
        piece_get(pos, Square::B1) == PieceType::NONE &&
        piece_get(pos, Square::C1) == PieceType::NONE &&
        piece_get(pos, Square::D1) == PieceType::NONE &&
        attacked(pos, Square::E1, Colour::THEM) == false &&
        attacked(pos, Square::D1, Colour::THEM) == false &&
        attacked(pos, Square::C1, Colour::THEM) == false) {
        UCI_ASSERT(pos.colour[US] & pos.pieces[PieceType::ROOK] & U64_FILE_A &
               U64_RANK_1);
        UCI_ASSERT(pos.colour[US] & pos.pieces[PieceType::KING] & U64_FILE_E &
               U64_RANK_1);

        movelist[num_moves] =
            Move(Square::E1, Square::C1, QSC, PieceType::KING);
        num_moves++;
    }

    UCI_ASSERT(num_moves >= 0);
    UCI_ASSERT(num_moves < MAX_MOVES);

#ifndef NDEBUG
    for (int n = 0; n < num_moves; ++n) {
        UCI_ASSERT(pseudolegal_move(pos, movelist[n]) == true);
        UCI_ASSERT(movelist[n].captured() == PieceType::NONE);
        UCI_ASSERT(movelist[n].type() == MoveType::QUIET ||
               movelist[n].type() == MoveType::DOUBLE ||
               movelist[n].type() == MoveType::KSC ||
               movelist[n].type() == MoveType::QSC ||
               movelist[n].type() == MoveType::PROMO);
    }
#endif

    return num_moves;
}
