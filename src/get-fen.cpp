#include "fen.hpp"
#include "position.hpp"
#include "flip.hpp"

std::string get_fen(const Position &pos)
{
    std::string fen = "";

    Position npos = pos;

    if(npos.flipped == true)
    {
        flip(npos);
    }

    // Part 0 -- Piece locations
    int empty = 0;
    for(int sq = A8; sq >= 0; ++sq)
    {
        uint64_t bb = (uint64_t)1 << sq;

        if(npos.pieces[PieceType::PAWN] & bb)
        {
            if(empty > 0)
            {
                fen += std::to_string(empty);
            }
            empty = 0;
            if(npos.colour[US] & bb)
            {
                fen += "P";
            }
            else
            {
                fen += "p";
            }
        }
        else if(npos.pieces[PieceType::KNIGHT] & bb)
        {
            if(empty > 0)
            {
                fen += std::to_string(empty);
            }
            empty = 0;
            if(npos.colour[US] & bb)
            {
                fen += "N";
            }
            else
            {
                fen += "n";
            }
        }
        else if(npos.pieces[PieceType::BISHOP] & bb)
        {
            if(empty > 0)
            {
                fen += std::to_string(empty);
            }
            empty = 0;
            if(npos.colour[US] & bb)
            {
                fen += "B";
            }
            else
            {
                fen += "b";
            }
        }
        else if(npos.pieces[PieceType::ROOK] & bb)
        {
            if(empty > 0)
            {
                fen += std::to_string(empty);
            }
            empty = 0;
            if(npos.colour[US] & bb)
            {
                fen += "R";
            }
            else
            {
                fen += "r";
            }
        }
        else if(npos.pieces[PieceType::QUEEN] & bb)
        {
            if(empty > 0)
            {
                fen += std::to_string(empty);
            }
            empty = 0;
            if(npos.colour[US] & bb)
            {
                fen += "Q";
            }
            else
            {
                fen += "q";
            }
        }
        else if(npos.pieces[PieceType::KING] & bb)
        {
            if(empty > 0)
            {
                fen += std::to_string(empty);
            }
            empty = 0;
            if(npos.colour[US] & bb)
            {
                fen += "K";
            }
            else
            {
                fen += "k";
            }
        }
        else
        {
            empty++;
        }

        if(sq % 8 == 7)
        {
            if(empty > 0)
            {
                fen += std::to_string(empty);
            }
            empty = 0;
            if(sq > H1)
            {
                fen += "/";
            }
            sq -= 16;
        }
    }

    // Part 1 -- Side to move
    if(pos.flipped)
    {
        fen += " b";
    }
    else
    {
        fen += " w";
    }

    // Part 2 -- Castling permissions
    std::string part = "";
    if(npos.castling[usKSC] == true)   {part += "K";}
    if(npos.castling[usQSC] == true)   {part += "Q";}
    if(npos.castling[themKSC] == true) {part += "k";}
    if(npos.castling[themQSC] == true) {part += "q";}
    if(part == "") {part = "-";}
    fen += " " + part;

    // Part 3 -- En passant square
    if(pos.enpassant == Square::OFFSQ)
    {
        fen += " -";
    }
    else
    {
        fen += " " + SquareString[npos.enpassant];
    }

    // Part 4 -- Halfmove clock
    fen += " " + std::to_string(npos.halfmoves);

    // Part 5 -- Fullmove number
    fen += " " + std::to_string(npos.fullmoves);

    return fen;
}
