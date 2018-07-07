#include <cassert>
#include "legal.hpp"
#include "movegen.hpp"
#include "makemove.hpp"
#include "attacks.hpp"

bool legal_move(const Position &pos, const Move &move)
{
    Move moves[256];
    int num_moves = movegen(pos, moves);

    for(int i = 0; i < num_moves; ++i)
    {
        if(move == moves[i])
        {
            Position npos = pos;
            make_move(npos, moves[i]);
            return !check(npos, Colour::THEM);
        }
    }

    return false;
}

bool legal_move(const Position &pos, const std::string &move)
{
    Move moves[256];
    int num_moves = movegen(pos, moves);

    for(int i = 0; i < num_moves; ++i)
    {
        if(move == move_uci(moves[i], pos.flipped))
        {
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
bool pseudolegal_move(const Position &pos, const Move &move)
{
    const Square from = move_from(move);
    const Square to = move_to(move);
    const MoveType type = move_type(move);
    const PieceType piece = move_piece(move);
    const PieceType captured = move_captured(move);
    const uint64_t from_bb = 1ULL << from;
    const uint64_t to_bb = 1ULL << to;

    assert(move != NO_MOVE);
    assert(from != to);
    assert(from >= Square::A1);
    assert(from <= Square::H8);
    assert(to >= Square::A1);
    assert(to <= Square::H8);
    assert(   captured == PieceType::PAWN
           || captured == PieceType::KNIGHT
           || captured == PieceType::BISHOP
           || captured == PieceType::ROOK
           || captured == PieceType::QUEEN
           || captured == PieceType::NONE);
    assert(   piece == PieceType::PAWN
           || piece == PieceType::KNIGHT
           || piece == PieceType::BISHOP
           || piece == PieceType::ROOK
           || piece == PieceType::QUEEN
           || piece == PieceType::KING);
    assert(   type == MoveType::QUIET
           || type == MoveType::DOUBLE
           || type == MoveType::CAPTURE
           || type == MoveType::ENPASSANT
           || type == MoveType::KSC
           || type == MoveType::QSC
           || type == MoveType::PROMO
           || type == MoveType::PROMO_CAPTURE);

    // Our piece needs to be there
    if(!(from_bb & pos.pieces[piece] & pos.colour[Colour::US]))
    {
        return false;
    }

    // We captured something
    if(captured != PieceType::NONE)
    {
        assert(   captured == PieceType::PAWN
               || captured == PieceType::KNIGHT
               || captured == PieceType::BISHOP
               || captured == PieceType::ROOK
               || captured == PieceType::QUEEN);
        assert(   type == MoveType::CAPTURE
               || type == MoveType::ENPASSANT
               || type == MoveType::PROMO_CAPTURE);

        if(type == MoveType::ENPASSANT)
        {
            assert(piece == PieceType::PAWN);
            assert(captured == PieceType::PAWN);

            // Enpassant has to be legal
            // This will tell us that the destination square is empty
            // Also that there's a pawn just below
            if(pos.enpassant != to)
            {
                return false;
            }
        }
        else
        {
            // The captured piece needs to be there
            if(!(to_bb & pos.pieces[captured] & pos.colour[Colour::THEM]))
            {
                return false;
            }
        }
    }
    // We didn't capture anything
    else
    {
        assert(captured == PieceType::NONE);
        assert(   type == MoveType::QUIET
               || type == MoveType::DOUBLE
               || type == MoveType::KSC
               || type == MoveType::QSC
               || type == MoveType::PROMO);

        // The destination square has to be empty
        if(to_bb & (pos.colour[Colour::US] | pos.colour[Colour::THEM]))
        {
            return false;
        }

        if(type == MoveType::DOUBLE)
        {
            assert(piece == PieceType::PAWN);

            // We can't double pawn move through another piece
            if((to_bb>>8) & (pos.colour[Colour::US] | pos.colour[Colour::THEM]))
            {
                return false;
            }
        }
    }

    if(type == MoveType::KSC)
    {
        assert(piece == PieceType::KING);
        assert(captured == PieceType::NONE);
        assert(to_bb == U64_G1);
        assert(from_bb == U64_E1);

        // We can only castle if we have permission to
        // This will tell us that the king & rook are where they're meant to be
        // We can't castle if pieces are in the way
        // We can't castle in or through check
        if(   pos.castling[Castling::usKSC] == false
           || !(U64_E1 & (pos.pieces[PieceType::KING] & pos.colour[Colour::US]))
           || !(U64_H1 & (pos.pieces[PieceType::ROOK] & pos.colour[Colour::US]))
           || (U64_F1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM]))
           || (U64_G1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM]))
           || attacked(pos, Square::E1, Colour::THEM) == true
           || attacked(pos, Square::F1, Colour::THEM) == true
           || attacked(pos, Square::G1, Colour::THEM) == true)
        {
            return false;
        }
    }
    if(type == MoveType::QSC)
    {
        assert(piece == PieceType::KING);
        assert(captured == PieceType::NONE);
        assert(to_bb == U64_C1);
        assert(from_bb == U64_E1);

        if(   pos.castling[Castling::usQSC] == false
           || !(U64_E1 & (pos.pieces[PieceType::KING] & pos.colour[Colour::US]))
           || !(U64_A1 & (pos.pieces[PieceType::ROOK] & pos.colour[Colour::US]))
           || (U64_D1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM]))
           || (U64_C1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM]))
           || (U64_B1 & (pos.colour[Colour::US] | pos.colour[Colour::THEM]))
           || attacked(pos, Square::E1, Colour::THEM) == true
           || attacked(pos, Square::D1, Colour::THEM) == true
           || attacked(pos, Square::C1, Colour::THEM) == true)
        {
            return false;
        }
    }

    return true;
}
