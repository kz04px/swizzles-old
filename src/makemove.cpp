#include "makemove.hpp"
#include <cassert>
#include <cstdint>
#include "flip.hpp"
#include "movegen.hpp"
#include "valid.hpp"
#include "zobrist.hpp"

void make_move(Position &pos, const Move m) {
    const Square from = move_from(m);
    const Square to = move_to(m);
    const MoveType type = move_type(m);
    const PieceType piece = move_piece(m);
    const PieceType captured = move_captured(m);

    assert(Square::A1 <= from && from <= Square::H8);
    assert(Square::A1 <= to && to <= Square::H8);
    assert(from != to);
    assert(piece != PieceType::NONE);
    assert(captured != PieceType::KING);

    pos.enpassant = Square::OFFSQ;
    pos.fullmoves += pos.flipped;

    // Remove the piece
    pos.pieces[piece] ^= 1ULL << from;
    pos.colour[Colour::US] ^= 1ULL << from;

    // Place the piece
    pos.pieces[piece] ^= 1ULL << to;
    pos.colour[Colour::US] ^= 1ULL << to;

    if (type == MoveType::CAPTURE) {
        assert(captured != PieceType::NONE);

        // Remove the captured piece
        pos.pieces[captured] ^= 1ULL << to;
        pos.colour[Colour::THEM] ^= 1ULL << to;
    } else if (type == MoveType::DOUBLE) {
        assert(piece == PieceType::PAWN);
        assert(captured == PieceType::NONE);
        assert(Square::A4 <= to);
        assert(to <= Square::H4);
        assert(Square::A2 <= from);
        assert(from <= Square::H2);

        // Add the enpassant square
        pos.enpassant = Square(to - 8);
    } else if (type == MoveType::PROMO) {
        assert(piece == PieceType::PAWN);
        assert(captured == PieceType::NONE);
        assert(Square::A8 <= to);
        assert(to <= Square::H8);
        assert(Square::A7 <= from);
        assert(from <= Square::H7);

        // Guess we should remove the pawn
        pos.pieces[PieceType::PAWN] ^= 1ULL << to;
        // Add the promoted piece
        pos.pieces[move_promo(m)] ^= 1ULL << to;
    } else if (type == MoveType::PROMO_CAPTURE) {
        assert(piece == PieceType::PAWN);
        assert(captured != PieceType::NONE);
        assert(Square::A8 <= to);
        assert(to <= Square::H8);
        assert(Square::A7 <= from);
        assert(from <= Square::H7);

        // Guess we should remove the pawn
        pos.pieces[PieceType::PAWN] ^= 1ULL << to;
        // Add the promoted piece
        pos.pieces[move_promo(m)] ^= 1ULL << to;
        // Remove the captured piece
        pos.pieces[captured] ^= 1ULL << to;
        pos.colour[Colour::THEM] ^= 1ULL << to;
    } else if (type == MoveType::KSC) {
        assert(piece == PieceType::KING);
        assert(captured == PieceType::NONE);
        assert(pos.castling[usKSC] == true);

        // Remove the rook
        pos.pieces[PieceType::ROOK] ^= 1ULL << Square::H1;
        pos.colour[Colour::US] ^= 1ULL << Square::H1;
        // Put it back
        pos.pieces[PieceType::ROOK] ^= 1ULL << Square::F1;
        pos.colour[Colour::US] ^= 1ULL << Square::F1;
    } else if (type == MoveType::QSC) {
        assert(piece == PieceType::KING);
        assert(captured == PieceType::NONE);
        assert(pos.castling[usQSC] == true);

        // Remove the rook
        pos.pieces[PieceType::ROOK] ^= 1ULL << Square::A1;
        pos.colour[Colour::US] ^= 1ULL << Square::A1;
        // Put it back
        pos.pieces[PieceType::ROOK] ^= 1ULL << Square::D1;
        pos.colour[Colour::US] ^= 1ULL << Square::D1;
    } else if (type == MoveType::ENPASSANT) {
        assert(piece == PieceType::PAWN);
        assert(captured == PieceType::PAWN);

        // Remove the enemy pawn
        pos.pieces[PieceType::PAWN] ^= 1ULL << (to - 8);
        pos.colour[Colour::THEM] ^= 1ULL << (to - 8);
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
    assert(pos.history_size >= 1);
    assert(pos.history_size <= 128);

    assert(valid(pos) == true);
}

bool make_move(Position &pos, const std::string &move_string) {
    Move moves[256];
    int num_moves = movegen(pos, moves);

    for (int i = 0; i < num_moves; ++i) {
        if (move_uci(moves[i], pos.flipped) == move_string) {
            make_move(pos, moves[i]);
            return true;
        }
    }

    return false;
}
