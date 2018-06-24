#include "fen.hpp"
#include "position.hpp"
#include "invalid.hpp"
#include "flip.hpp"
#include "zobrist.hpp"
#include "other.hpp"

#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

bool set_fen(Position &pos, std::string fen)
{
    // Clear the board
    pos.colour[US] = 0ULL;
    pos.colour[THEM] = 0ULL;
    pos.pieces[PieceType::PAWN] = 0ULL;
    pos.pieces[PieceType::KNIGHT] = 0ULL;
    pos.pieces[PieceType::BISHOP] = 0ULL;
    pos.pieces[PieceType::ROOK] = 0ULL;
    pos.pieces[PieceType::QUEEN] = 0ULL;
    pos.pieces[PieceType::KING] = 0ULL;
    pos.castling[usKSC] = false;
    pos.castling[usQSC] = false;
    pos.castling[themKSC] = false;
    pos.castling[themQSC] = false;
    pos.enpassant = Square::OFFSQ;
    pos.flipped = false;
    pos.halfmoves = 0;
    pos.fullmoves = 0;
    pos.history_size = 0;

    // Protocol requires we parse "startpos"
    if(fen == "startpos")
    {
        fen = STARTPOS;
    }

    // Split the fen into its 6 parts
    std::vector<std::string> parts = split(fen, ' ');
    if(parts.size() != 6)
    {
        return false;
    }

    // Part 0 -- Piece locations
    int sq = A8;
    for(auto &c : parts[0])
    {
        uint64_t bb = 1ULL << sq;

        switch(c)
        {
            case 'P':
                pos.pieces[PAWN] ^= bb;
                pos.colour[US] ^= bb;
                break;
            case 'N':
                pos.pieces[KNIGHT] ^= bb;
                pos.colour[US] ^= bb;
                break;
            case 'B':
                pos.pieces[BISHOP] ^= bb;
                pos.colour[US] ^= bb;
                break;
            case 'R':
                pos.pieces[ROOK] ^= bb;
                pos.colour[US] ^= bb;
                break;
            case 'Q':
                pos.pieces[QUEEN] ^= bb;
                pos.colour[US] ^= bb;
                break;
            case 'K':
                pos.pieces[KING] ^= bb;
                pos.colour[US] ^= bb;
                break;
            case 'p':
                pos.pieces[PAWN] ^= bb;
                pos.colour[THEM] ^= bb;
                break;
            case 'n':
                pos.pieces[KNIGHT] ^= bb;
                pos.colour[THEM] ^= bb;
                break;
            case 'b':
                pos.pieces[BISHOP] ^= bb;
                pos.colour[THEM] ^= bb;
                break;
            case 'r':
                pos.pieces[ROOK] ^= bb;
                pos.colour[THEM] ^= bb;
                break;
            case 'q':
                pos.pieces[QUEEN] ^= bb;
                pos.colour[THEM] ^= bb;
                break;
            case 'k':
                pos.pieces[KING] ^= bb;
                pos.colour[THEM] ^= bb;
                break;
            case '/':
                sq -= 17;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                sq += c - '1';
                break;
            default:
                break;
        }

        sq++;
    }

    // Part 1 -- Side to move
    if(parts[1] == "w" || parts[1] == "W")
    {
        pos.flipped = false;
    }
    else if(parts[1] == "b" || parts[1] == "B")
    {
        pos.flipped = true;
    }
    else
    {
        return false;
    }

    // Part 2 -- Castling permissions
    for(auto &c : parts[2])
    {
        switch(c)
        {
            case 'K':
                pos.castling[usKSC] = true;
                break;
            case 'Q':
                pos.castling[usQSC] = true;
                break;
            case 'k':
                pos.castling[themKSC] = true;
                break;
            case 'q':
                pos.castling[themQSC] = true;
                break;
            case '-':
                break;
            default:
                return false;
                break;
        }
    }

    // Part 3 -- En passant square
    if(parts[3] == "-")
    {
        pos.enpassant = OFFSQ;
    }
    else
    {
        for(int sq = 0; sq < 64; ++sq)
        {
            if(SquareString[sq] == parts[3])
            {
                pos.enpassant = Square(sq);
                break;
            }
        }
    }

    // Part 4 -- Halfmove clock
    pos.halfmoves = std::stoi(parts[4]);
    if(pos.halfmoves < 0)
    {
        return false;
    }

    // Part 5 -- Fullmove number
    pos.fullmoves = std::stoi(parts[5]);
    if(pos.fullmoves < 0)
    {
        return false;
    }

    // Flip the board to match the fen
    if(pos.flipped == true)
    {
        flip(pos);
        pos.flipped = true;
    }

    // Add position to history
    std::uint64_t hash = calculate_hash(pos);
    pos.history[pos.history_size] = hash;
    pos.history_size++;

    if(invalid(pos) == true)
    {
        return false;
    }

    return true;
}
