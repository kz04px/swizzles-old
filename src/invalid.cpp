#include "position.hpp"
#include "other.hpp"

int invalid(const Position &pos)
{
    // Piece overlaps
    if(pos.pieces[PieceType::PAWN]   & pos.pieces[PieceType::KNIGHT]) {return 1;}
    if(pos.pieces[PieceType::PAWN]   & pos.pieces[PieceType::BISHOP]) {return 2;}
    if(pos.pieces[PieceType::PAWN]   & pos.pieces[PieceType::ROOK])   {return 3;}
    if(pos.pieces[PieceType::PAWN]   & pos.pieces[PieceType::QUEEN])  {return 4;}
    if(pos.pieces[PieceType::PAWN]   & pos.pieces[PieceType::KING])   {return 5;}
    if(pos.pieces[PieceType::KNIGHT] & pos.pieces[PieceType::BISHOP]) {return 6;}
    if(pos.pieces[PieceType::KNIGHT] & pos.pieces[PieceType::ROOK])   {return 7;}
    if(pos.pieces[PieceType::KNIGHT] & pos.pieces[PieceType::QUEEN])  {return 8;}
    if(pos.pieces[PieceType::KNIGHT] & pos.pieces[PieceType::KING])   {return 9;}
    if(pos.pieces[PieceType::BISHOP] & pos.pieces[PieceType::ROOK])   {return 10;}
    if(pos.pieces[PieceType::BISHOP] & pos.pieces[PieceType::QUEEN])  {return 11;}
    if(pos.pieces[PieceType::BISHOP] & pos.pieces[PieceType::KING])   {return 12;}
    if(pos.pieces[PieceType::ROOK]   & pos.pieces[PieceType::QUEEN])  {return 13;}
    if(pos.pieces[PieceType::ROOK]   & pos.pieces[PieceType::KING])   {return 14;}
    if(pos.pieces[PieceType::QUEEN]  & pos.pieces[PieceType::KING])   {return 15;}

    // Colour overlaps
    if(pos.colour[US] & pos.colour[THEM]) {return 16;}

    // Piece counts
    if(popcountll(pos.pieces[PieceType::KING] & pos.colour[US]) != 1)   {return 17;}
    if(popcountll(pos.pieces[PieceType::KING] & pos.colour[THEM]) != 1) {return 18;}
    if(popcountll(pos.pieces[PieceType::PAWN] & pos.colour[US]) > 8)    {return 19;}
    if(popcountll(pos.pieces[PieceType::PAWN] & pos.colour[THEM]) > 8)  {return 20;}
    if(popcountll(pos.colour[US]) > 16)   {return 21;}
    if(popcountll(pos.colour[THEM]) > 16) {return 22;}

    // En passant square
    //if(enpassant != Square::OFFSQ && (enpassant < Square::A1 || enpassant > Square::H8)) {return 23;}
    if(pos.enpassant != Square::OFFSQ)
    {
        if(pos.enpassant < Square::A6 || pos.enpassant > Square::H6)
        {
            return 23;
        }
    }

    // Pawns in the wrong place
    if(pos.pieces[PieceType::PAWN] & U64_RANK_1) {return 24;}
    if(pos.pieces[PieceType::PAWN] & U64_RANK_8) {return 25;}

    return 0;
}
