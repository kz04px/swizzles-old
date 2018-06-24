#include <iostream>
#include "display.hpp"
#include "position.hpp"
#include "flip.hpp"
#include "phase.hpp"
#include "zobrist.hpp"
#include "other.hpp"

void display(const Position &pos)
{
    Position npos = pos;

    if(pos.flipped == true)
    {
        flip(npos);
    }

    for(int sq = A8; sq >= 0; ++sq)
    {
        uint64_t bb = (uint64_t)1 << sq;

        if(npos.pieces[PieceType::PAWN] & bb)
        {
            if(npos.colour[US] & bb)
            {
                std::cout << "P";
            }
            else
            {
                std::cout << "p";
            }
        }
        else if(npos.pieces[PieceType::KNIGHT] & bb)
        {
            if(npos.colour[US] & bb)
            {
                std::cout << "N";
            }
            else
            {
                std::cout << "n";
            }
        }
        else if(npos.pieces[PieceType::BISHOP] & bb)
        {
            if(npos.colour[US] & bb)
            {
                std::cout << "B";
            }
            else
            {
                std::cout << "b";
            }
        }
        else if(npos.pieces[PieceType::ROOK] & bb)
        {
            if(npos.colour[US] & bb)
            {
                std::cout << "R";
            }
            else
            {
                std::cout << "r";
            }
        }
        else if(npos.pieces[PieceType::QUEEN] & bb)
        {
            if(npos.colour[US] & bb)
            {
                std::cout << "Q";
            }
            else
            {
                std::cout << "q";
            }
        }
        else if(npos.pieces[PieceType::KING] & bb)
        {
            if(npos.colour[US] & bb)
            {
                std::cout << "K";
            }
            else
            {
                std::cout << "k";
            }
        }
        else
        {
            std::cout << "-";
        }

        if(sq % 8 == 7)
        {
            std::cout << std::endl;
            sq -= 16;
        }
    }

    std::string part = "";
    if(npos.castling[usKSC] == true) {part += "K";}
    if(npos.castling[usQSC] == true) {part += "Q";}
    if(npos.castling[themKSC] == true) {part += "k";}
    if(npos.castling[themQSC] == true) {part += "q";}
    if(part == "") {part += "-";}

    std::cout << "Turn: " << (pos.flipped == true ? "b" : "w") << std::endl;
    std::cout << "Castling:  " << part << std::endl;
    std::cout << "Enpassant: " << (npos.enpassant == Square::OFFSQ ? "-" : SquareString[npos.enpassant]) << std::endl;
    std::cout << "Halfmoves: " << pos.halfmoves << std::endl;
    std::cout << "Fullmoves: " << pos.fullmoves << std::endl;
    std::cout << "Phase: " << phase(pos) << std::endl;
    std::cout << "Hash: " << calculate_hash(pos) << std::endl;
}
