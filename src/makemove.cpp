#include "makemove.hpp"
#include "assert.hpp"
#include <cstdint>
#include "movegen.hpp"
#include "position.hpp"
#include "types.hpp"
#include "zobrist.hpp"

void make_move(Position &pos, const Move m) {
    const Square from = m.from();
    const Square to = m.to();
    const MoveType type = m.type();
    const PieceType piece = m.piece();
    const PieceType captured = m.captured();

    UCI_ASSERT(Square::A1 <= from && from <= Square::H8);
    UCI_ASSERT(Square::A1 <= to && to <= Square::H8);
    UCI_ASSERT(from != to);
    UCI_ASSERT(piece != PieceType::NONE);
    UCI_ASSERT(captured != PieceType::KING);

    pos.enpassant = Square::A1;
    pos.fullmoves += pos.flipped;

    // Remove the piece
    pos.pieces[piece] ^= 1ULL << from;
    pos.colour[Colour::US] ^= 1ULL << from;

    // Place the piece
    pos.pieces[piece] ^= 1ULL << to;
    pos.colour[Colour::US] ^= 1ULL << to;

    if (type == MoveType::CAPTURE) {
        UCI_ASSERT(captured != PieceType::NONE);

        // Remove the captured piece
        pos.pieces[captured] ^= 1ULL << to;
        pos.colour[Colour::THEM] ^= 1ULL << to;
    } else if (type == MoveType::DOUBLE) {
        UCI_ASSERT(piece == PieceType::PAWN);
        UCI_ASSERT(captured == PieceType::NONE);
        UCI_ASSERT(Square::A4 <= to);
        UCI_ASSERT(to <= Square::H4);
        UCI_ASSERT(Square::A2 <= from);
        UCI_ASSERT(from <= Square::H2);

        // Add the enpassant square
        pos.enpassant = Square(to - 8);
    } else if (type == MoveType::PROMO) {
        UCI_ASSERT(piece == PieceType::PAWN);
        UCI_ASSERT(captured == PieceType::NONE);
        UCI_ASSERT(Square::A8 <= to);
        UCI_ASSERT(to <= Square::H8);
        UCI_ASSERT(Square::A7 <= from);
        UCI_ASSERT(from <= Square::H7);

        // Guess we should remove the pawn
        pos.pieces[PieceType::PAWN] ^= 1ULL << to;
        // Add the promoted piece
        pos.pieces[m.promo()] ^= 1ULL << to;
    } else if (type == MoveType::PROMO_CAPTURE) {
        UCI_ASSERT(piece == PieceType::PAWN);
        UCI_ASSERT(captured != PieceType::NONE);
        UCI_ASSERT(Square::A8 <= to);
        UCI_ASSERT(to <= Square::H8);
        UCI_ASSERT(Square::A7 <= from);
        UCI_ASSERT(from <= Square::H7);

        // Guess we should remove the pawn
        pos.pieces[PieceType::PAWN] ^= 1ULL << to;
        // Add the promoted piece
        pos.pieces[m.promo()] ^= 1ULL << to;
        // Remove the captured piece
        pos.pieces[captured] ^= 1ULL << to;
        pos.colour[Colour::THEM] ^= 1ULL << to;
    } else if (type == MoveType::KSC) {
        UCI_ASSERT(piece == PieceType::KING);
        UCI_ASSERT(captured == PieceType::NONE);
        UCI_ASSERT(pos.castling[usKSC] == true);

        // Remove the rook
        pos.pieces[PieceType::ROOK] ^= 1ULL << Square::H1;
        pos.colour[Colour::US] ^= 1ULL << Square::H1;
        // Put it back
        pos.pieces[PieceType::ROOK] ^= 1ULL << Square::F1;
        pos.colour[Colour::US] ^= 1ULL << Square::F1;
    } else if (type == MoveType::QSC) {
        UCI_ASSERT(piece == PieceType::KING);
        UCI_ASSERT(captured == PieceType::NONE);
        UCI_ASSERT(pos.castling[usQSC] == true);

        // Remove the rook
        pos.pieces[PieceType::ROOK] ^= 1ULL << Square::A1;
        pos.colour[Colour::US] ^= 1ULL << Square::A1;
        // Put it back
        pos.pieces[PieceType::ROOK] ^= 1ULL << Square::D1;
        pos.colour[Colour::US] ^= 1ULL << Square::D1;
    } else if (type == MoveType::ENPASSANT) {
        UCI_ASSERT(piece == PieceType::PAWN);
        UCI_ASSERT(captured == PieceType::PAWN);

        // Remove the enemy pawn
        pos.pieces[PieceType::PAWN] ^= 1ULL << (to - 8);
        pos.colour[Colour::THEM] ^= 1ULL << (to - 8);
    } else {
        UCI_ASSERT(type == MoveType::QUIET);
    }

    // Castling permissions
    pos.castling[usKSC] &=
        !(to == Square::H1 || from == Square::E1 || from == Square::H1);
    pos.castling[usQSC] &=
        !(to == Square::A1 || from == Square::E1 || from == Square::A1);
    pos.castling[themKSC] &=
        !(to == Square::H8 || from == Square::E8 || from == Square::H8);
    pos.castling[themQSC] &=
        !(to == Square::A8 || from == Square::E8 || from == Square::A8);

    flip(pos);

    pos.halfmoves++;
    if (piece == PieceType::PAWN || type == MoveType::CAPTURE) {
        pos.halfmoves = 0;
    }
    if (piece == PieceType::PAWN || type == MoveType::CAPTURE ||
        type == MoveType::KSC || type == MoveType::QSC) {
        pos.history_size = 0;
    }

    std::uint64_t hash = calculate_hash(pos);
    pos.history[pos.history_size] = hash;
    pos.history_size++;
    UCI_ASSERT(pos.history_size >= 1);
    UCI_ASSERT(pos.history_size <= 128);

    UCI_ASSERT(legal_position(pos) == true);
}

bool make_move(Position &pos, const std::string &move_string) {
    Move moves[MAX_MOVES];
    int num_moves = movegen(pos, moves);

    for (int i = 0; i < num_moves; ++i) {
        if (moves[i].uci(pos.flipped) == move_string) {
            make_move(pos, moves[i]);
            return true;
        }
    }

    return false;
}
