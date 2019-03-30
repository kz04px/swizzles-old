#include "flip.hpp"
#include "other.hpp"
#include "position.hpp"
#include "types.hpp"

void flip(Position &pos) {
    // Pieces
    pos.pieces[PieceType::PAWN] = swapll(pos.pieces[PieceType::PAWN]);
    pos.pieces[PieceType::KNIGHT] = swapll(pos.pieces[PieceType::KNIGHT]);
    pos.pieces[PieceType::BISHOP] = swapll(pos.pieces[PieceType::BISHOP]);
    pos.pieces[PieceType::ROOK] = swapll(pos.pieces[PieceType::ROOK]);
    pos.pieces[PieceType::QUEEN] = swapll(pos.pieces[PieceType::QUEEN]);
    pos.pieces[PieceType::KING] = swapll(pos.pieces[PieceType::KING]);

    // Colours
    const uint64_t store = swapll(pos.colour[Colour::US]);
    pos.colour[Colour::US] = swapll(pos.colour[Colour::THEM]);
    pos.colour[Colour::THEM] = store;

    // Enpassant
    if (pos.enpassant != Square::OFFSQ) {
        pos.enpassant = Square(pos.enpassant ^ 56);
    }

    // Castling - King side
    bool store2 = pos.castling[0];
    pos.castling[0] = pos.castling[2];
    pos.castling[2] = store2;

    // Castling - Queen side
    store2 = pos.castling[1];
    pos.castling[1] = pos.castling[3];
    pos.castling[3] = store2;

    pos.flipped = !pos.flipped;
}
