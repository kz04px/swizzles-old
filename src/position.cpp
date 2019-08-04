#include "position.hpp"
#include "assert.hpp"
#include <cstdint>
#include "attacks.hpp"
#include "makemove.hpp"
#include "movegen.hpp"
#include "other.hpp"
#include "types.hpp"
#include "zobrist.hpp"

PieceType piece_get(const Position &pos, const Square sq) {
    const uint64_t bb = 1ULL << sq;

    for (int i = PieceType::PAWN; i <= PieceType::KING; ++i) {
        if (pos.pieces[i] & bb) {
            return PieceType(i);
        }
    }

    return PieceType::NONE;
}

bool operator==(const Position &a, const Position &b) {
    // Flipped
    if (a.flipped != b.flipped) {
        return false;
    }

    // Pieces
    if (a.pieces[PieceType::PAWN] != b.pieces[PieceType::PAWN]) {
        return false;
    }
    if (a.pieces[PieceType::KNIGHT] != b.pieces[PieceType::KNIGHT]) {
        return false;
    }
    if (a.pieces[PieceType::BISHOP] != b.pieces[PieceType::BISHOP]) {
        return false;
    }
    if (a.pieces[PieceType::ROOK] != b.pieces[PieceType::ROOK]) {
        return false;
    }
    if (a.pieces[PieceType::QUEEN] != b.pieces[PieceType::QUEEN]) {
        return false;
    }
    if (a.pieces[PieceType::KING] != b.pieces[PieceType::KING]) {
        return false;
    }

    // Colours
    if (a.colour[Colour::US] != b.colour[Colour::US]) {
        return false;
    }
    if (a.colour[Colour::THEM] != b.colour[Colour::THEM]) {
        return false;
    }

    // Castling
    if (a.castling[usKSC] != b.castling[usKSC]) {
        return false;
    }
    if (a.castling[usQSC] != b.castling[usQSC]) {
        return false;
    }
    if (a.castling[themKSC] != b.castling[themKSC]) {
        return false;
    }
    if (a.castling[themQSC] != b.castling[themQSC]) {
        return false;
    }

    // Enpassant
    if (a.enpassant != b.enpassant) {
        return false;
    }

    // Move counts
    if (a.halfmoves != b.halfmoves) {
        return false;
    }
    if (a.fullmoves != b.fullmoves) {
        return false;
    }

    return true;
}

bool operator!=(const Position &a, const Position &b) {
    return !(a == b);
}

int repetitions(const Position &pos) {
    int count = 0;
    for (int i = pos.history_size - 3; i >= 0; --i) {
        if (pos.history[i] == pos.history[pos.history_size - 1]) {
            count++;
        }
    }
    return count;
}

bool is_fifty_moves(const Position &pos) {
    return pos.halfmoves >= 100;
}

int is_endgame(const Position &pos) {
    return popcountll(
               pos.colour[pos.flipped] &
               (pos.pieces[PieceType::KNIGHT] | pos.pieces[PieceType::BISHOP] |
                pos.pieces[PieceType::ROOK] | pos.pieces[PieceType::QUEEN])) <=
           2;
}

bool legal_move(const Position &pos, const Move &move) {
    Move moves[MAX_MOVES];
    int num_moves = movegen(pos, moves);

    for (int i = 0; i < num_moves; ++i) {
        if (move == moves[i]) {
            Position npos = pos;
            make_move(npos, moves[i]);
            return !check(npos, Colour::THEM);
        }
    }

    return false;
}

bool legal_move(const Position &pos, const std::string &move) {
    Move moves[MAX_MOVES];
    int num_moves = movegen(pos, moves);

    for (int i = 0; i < num_moves; ++i) {
        if (move == moves[i].uci(pos.flipped)) {
            Position npos = pos;
            make_move(npos, moves[i]);
            return !check(npos, Colour::THEM);
        }
    }

    return false;
}

// This is only for pseudo legality, moves might still put us into check
// This function should only run on moves we've generated ourselves
// That means that certain things can be taken for granted:
// - Captures will have a captured piece
// - Promotions will be to the 8th rank
// - The destination and source squares make sense (e2e4 not e2f7)
// - Etc
// We only need to check that the move is legal in the current position:
// - Don't castle while in or through check
// - Don't double pawn move through another piece
// - Captured pieces need to be there
// - Etc
bool pseudolegal_move(const Position &pos, const Move &move) {
    const Square from = move.from();
    const Square to = move.to();
    const MoveType type = move.type();
    const PieceType piece = move.piece();
    const PieceType captured = move.captured();
    const uint64_t from_bb = 1ULL << from;
    const uint64_t to_bb = 1ULL << to;

    UCI_ASSERT(move != NO_MOVE);
    UCI_ASSERT(from != to);
    UCI_ASSERT(from >= Square::A1);
    UCI_ASSERT(from <= Square::H8);
    UCI_ASSERT(to >= Square::A1);
    UCI_ASSERT(to <= Square::H8);
    UCI_ASSERT(captured == PieceType::PAWN || captured == PieceType::KNIGHT ||
           captured == PieceType::BISHOP || captured == PieceType::ROOK ||
           captured == PieceType::QUEEN || captured == PieceType::NONE);
    UCI_ASSERT(piece == PieceType::PAWN || piece == PieceType::KNIGHT ||
           piece == PieceType::BISHOP || piece == PieceType::ROOK ||
           piece == PieceType::QUEEN || piece == PieceType::KING);
    UCI_ASSERT(type == MoveType::QUIET || type == MoveType::DOUBLE ||
           type == MoveType::CAPTURE || type == MoveType::ENPASSANT ||
           type == MoveType::KSC || type == MoveType::QSC ||
           type == MoveType::PROMO || type == MoveType::PROMO_CAPTURE);

    // Our piece needs to be there
    if (!(from_bb & pos.pieces[piece] & pos.colour[Colour::US])) {
        return false;
    }

    // We captured something
    if (captured != PieceType::NONE) {
        UCI_ASSERT(captured == PieceType::PAWN || captured == PieceType::KNIGHT ||
               captured == PieceType::BISHOP || captured == PieceType::ROOK ||
               captured == PieceType::QUEEN);
        UCI_ASSERT(type == MoveType::CAPTURE || type == MoveType::ENPASSANT ||
               type == MoveType::PROMO_CAPTURE);

        if (type == MoveType::ENPASSANT) {
            UCI_ASSERT(piece == PieceType::PAWN);
            UCI_ASSERT(captured == PieceType::PAWN);

            // Enpassant has to be legal
            // This will tell us that the destination square is empty
            // Also that there's a pawn just below
            if (pos.enpassant != to) {
                return false;
            }
        } else {
            // The captured piece needs to be there
            if (!(to_bb & pos.pieces[captured] & pos.colour[Colour::THEM])) {
                return false;
            }
        }
    }
    // We didn't capture anything
    else {
        UCI_ASSERT(captured == PieceType::NONE);
        UCI_ASSERT(type == MoveType::QUIET || type == MoveType::DOUBLE ||
               type == MoveType::KSC || type == MoveType::QSC ||
               type == MoveType::PROMO);

        // The destination square has to be empty
        if (to_bb & (pos.colour[Colour::US] | pos.colour[Colour::THEM])) {
            return false;
        }

        if (type == MoveType::DOUBLE) {
            UCI_ASSERT(piece == PieceType::PAWN);

            // We can't double pawn move through another piece
            if ((to_bb >> 8) &
                (pos.colour[Colour::US] | pos.colour[Colour::THEM])) {
                return false;
            }
        }
    }

    if (type == MoveType::KSC) {
        UCI_ASSERT(piece == PieceType::KING);
        UCI_ASSERT(captured == PieceType::NONE);
        UCI_ASSERT(to_bb == U64_G1);
        UCI_ASSERT(from_bb == U64_E1);

        // We can only castle if we have permission to
        // This will tell us that the king & rook are where they're meant to be
        // We can't castle if pieces are in the way
        // We can't castle in or through check
        if (pos.castling[Castling::usKSC] == false ||
            !(U64_E1 &
              (pos.pieces[PieceType::KING] & pos.colour[Colour::US])) ||
            !(U64_H1 &
              (pos.pieces[PieceType::ROOK] & pos.colour[Colour::US])) ||
            (U64_F1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM])) ||
            (U64_G1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM])) ||
            attacked(pos, Square::E1, Colour::THEM) == true ||
            attacked(pos, Square::F1, Colour::THEM) == true ||
            attacked(pos, Square::G1, Colour::THEM) == true) {
            return false;
        }
    }
    if (type == MoveType::QSC) {
        UCI_ASSERT(piece == PieceType::KING);
        UCI_ASSERT(captured == PieceType::NONE);
        UCI_ASSERT(to_bb == U64_C1);
        UCI_ASSERT(from_bb == U64_E1);

        if (pos.castling[Castling::usQSC] == false ||
            !(U64_E1 &
              (pos.pieces[PieceType::KING] & pos.colour[Colour::US])) ||
            !(U64_A1 &
              (pos.pieces[PieceType::ROOK] & pos.colour[Colour::US])) ||
            (U64_D1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM])) ||
            (U64_C1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM])) ||
            (U64_B1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM])) ||
            attacked(pos, Square::E1, Colour::THEM) == true ||
            attacked(pos, Square::D1, Colour::THEM) == true ||
            attacked(pos, Square::C1, Colour::THEM) == true) {
            return false;
        }
    }

    return true;
}

bool legal_position(const Position &pos) {
    // Piece overlaps
    if (pos.pieces[PieceType::PAWN] & pos.pieces[PieceType::KNIGHT]) {
        return false;
    }
    if (pos.pieces[PieceType::PAWN] & pos.pieces[PieceType::BISHOP]) {
        return false;
    }
    if (pos.pieces[PieceType::PAWN] & pos.pieces[PieceType::ROOK]) {
        return false;
    }
    if (pos.pieces[PieceType::PAWN] & pos.pieces[PieceType::QUEEN]) {
        return false;
    }
    if (pos.pieces[PieceType::PAWN] & pos.pieces[PieceType::KING]) {
        return false;
    }
    if (pos.pieces[PieceType::KNIGHT] & pos.pieces[PieceType::BISHOP]) {
        return false;
    }
    if (pos.pieces[PieceType::KNIGHT] & pos.pieces[PieceType::ROOK]) {
        return false;
    }
    if (pos.pieces[PieceType::KNIGHT] & pos.pieces[PieceType::QUEEN]) {
        return false;
    }
    if (pos.pieces[PieceType::KNIGHT] & pos.pieces[PieceType::KING]) {
        return false;
    }
    if (pos.pieces[PieceType::BISHOP] & pos.pieces[PieceType::ROOK]) {
        return false;
    }
    if (pos.pieces[PieceType::BISHOP] & pos.pieces[PieceType::QUEEN]) {
        return false;
    }
    if (pos.pieces[PieceType::BISHOP] & pos.pieces[PieceType::KING]) {
        return false;
    }
    if (pos.pieces[PieceType::ROOK] & pos.pieces[PieceType::QUEEN]) {
        return false;
    }
    if (pos.pieces[PieceType::ROOK] & pos.pieces[PieceType::KING]) {
        return false;
    }
    if (pos.pieces[PieceType::QUEEN] & pos.pieces[PieceType::KING]) {
        return false;
    }

    // Colour overlaps
    if (pos.colour[US] & pos.colour[THEM]) {
        return false;
    }

    // Piece counts
    if (popcountll(pos.pieces[PieceType::KING] & pos.colour[US]) != 1) {
        return false;
    }
    if (popcountll(pos.pieces[PieceType::KING] & pos.colour[THEM]) != 1) {
        return false;
    }
    if (popcountll(pos.pieces[PieceType::PAWN] & pos.colour[US]) > 8) {
        return false;
    }
    if (popcountll(pos.pieces[PieceType::PAWN] & pos.colour[THEM]) > 8) {
        return false;
    }
    if (popcountll(pos.colour[US]) > 16) {
        return false;
    }
    if (popcountll(pos.colour[THEM]) > 16) {
        return false;
    }

    // En passant square
    if (pos.enpassant != Square::A1) {
        if (pos.enpassant < Square::A6 || pos.enpassant > Square::H6) {
            return false;
        }
    }

    // Pawns in the wrong place
    if (pos.pieces[PieceType::PAWN] & U64_RANK_1) {
        return false;
    }
    if (pos.pieces[PieceType::PAWN] & U64_RANK_8) {
        return false;
    }

    // At least the current position has to be in the history
    if (pos.history_size < 1) {
        return false;
    }

    // These need to match
    if (pos.history[pos.history_size - 1] != zobrist::calculate_hash(pos)) {
        return false;
    }

    // Needs to be a pawn below the EP square
    if (pos.enpassant != Square::A1) {
        if (((1ULL << (pos.enpassant - 8)) &
             (pos.pieces[PieceType::PAWN] & pos.colour[Colour::THEM])) ==
            0ULL) {
            return false;
        }
    }

    // Castling permissions
    if (pos.castling[Castling::usKSC] == true) {
        if ((pos.pieces[PieceType::KING] & pos.colour[Colour::US] & U64_E1) ==
            0ULL) {
            return false;
        }
        if ((pos.pieces[PieceType::ROOK] & pos.colour[Colour::US] & U64_H1) ==
            0ULL) {
            return false;
        }
    }
    if (pos.castling[Castling::usQSC] == true) {
        if ((pos.pieces[PieceType::KING] & pos.colour[Colour::US] & U64_E1) ==
            0ULL) {
            return false;
        }
        if ((pos.pieces[PieceType::ROOK] & pos.colour[Colour::US] & U64_A1) ==
            0ULL) {
            return false;
        }
    }

    return true;
}

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
    if (pos.enpassant != Square::A1) {
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
