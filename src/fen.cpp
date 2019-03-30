#include "fen.hpp"
#include <sstream>
#include "other.hpp"
#include "position.hpp"
#include "zobrist.hpp"

bool set_fen(Position &pos, const std::string &fen) {
    // Parse "startpos"
    if (fen == "startpos") {
        return set_fen(
            pos, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }

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

    std::string word;
    std::stringstream ss{fen};

    // Part 1 -- Piece locations
    if (ss >> word) {
        int sq = A8;
        for (auto &c : word) {
            uint64_t bb = 1ULL << sq;

            switch (c) {
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
    }

    // Part 2 -- Side to move
    if (ss >> word) {
        if (word == "w" || word == "W") {
            pos.flipped = false;
        } else if (word == "b" || word == "B") {
            pos.flipped = true;
        } else {
            return false;
        }
    }

    // Part 3 -- Castling permissions
    if (ss >> word) {
        for (auto &c : word) {
            switch (c) {
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
    }

    // Part 4 -- En passant square
    if (ss >> word) {
        if (word == "-") {
            pos.enpassant = OFFSQ;
        } else {
            for (int sq = 0; sq < 64; ++sq) {
                if (SquareString[sq] == word) {
                    pos.enpassant = Square(sq);
                    break;
                }
            }
        }
    }

    // Part 5 -- Halfmove clock
    ss >> pos.halfmoves;

    // Part 6 -- Fullmove number
    ss >> pos.fullmoves;

    // Flip the board to match the fen
    if (pos.flipped == true) {
        flip(pos);
        pos.flipped = true;
    }

    // Add position to history
    std::uint64_t hash = calculate_hash(pos);
    pos.history[pos.history_size] = hash;
    pos.history_size++;

    return legal_position(pos);
}

std::string get_fen(const Position &pos) {
    std::string fen = "";

    Position npos = pos;

    if (npos.flipped == true) {
        flip(npos);
    }

    // Part 1 -- Piece locations
    int empty = 0;
    for (int sq = A8; sq >= 0; ++sq) {
        uint64_t bb = (uint64_t)1 << sq;

        if (npos.pieces[PieceType::PAWN] & bb) {
            if (empty > 0) {
                fen += std::to_string(empty);
            }
            empty = 0;
            if (npos.colour[US] & bb) {
                fen += "P";
            } else {
                fen += "p";
            }
        } else if (npos.pieces[PieceType::KNIGHT] & bb) {
            if (empty > 0) {
                fen += std::to_string(empty);
            }
            empty = 0;
            if (npos.colour[US] & bb) {
                fen += "N";
            } else {
                fen += "n";
            }
        } else if (npos.pieces[PieceType::BISHOP] & bb) {
            if (empty > 0) {
                fen += std::to_string(empty);
            }
            empty = 0;
            if (npos.colour[US] & bb) {
                fen += "B";
            } else {
                fen += "b";
            }
        } else if (npos.pieces[PieceType::ROOK] & bb) {
            if (empty > 0) {
                fen += std::to_string(empty);
            }
            empty = 0;
            if (npos.colour[US] & bb) {
                fen += "R";
            } else {
                fen += "r";
            }
        } else if (npos.pieces[PieceType::QUEEN] & bb) {
            if (empty > 0) {
                fen += std::to_string(empty);
            }
            empty = 0;
            if (npos.colour[US] & bb) {
                fen += "Q";
            } else {
                fen += "q";
            }
        } else if (npos.pieces[PieceType::KING] & bb) {
            if (empty > 0) {
                fen += std::to_string(empty);
            }
            empty = 0;
            if (npos.colour[US] & bb) {
                fen += "K";
            } else {
                fen += "k";
            }
        } else {
            empty++;
        }

        if (sq % 8 == 7) {
            if (empty > 0) {
                fen += std::to_string(empty);
            }
            empty = 0;
            if (sq > H1) {
                fen += "/";
            }
            sq -= 16;
        }
    }

    // Part 2 -- Side to move
    if (pos.flipped) {
        fen += " b";
    } else {
        fen += " w";
    }

    // Part 3 -- Castling permissions
    std::string part = "";
    if (npos.castling[usKSC] == true) {
        part += "K";
    }
    if (npos.castling[usQSC] == true) {
        part += "Q";
    }
    if (npos.castling[themKSC] == true) {
        part += "k";
    }
    if (npos.castling[themQSC] == true) {
        part += "q";
    }
    if (part == "") {
        part = "-";
    }
    fen += " " + part;

    // Part 4 -- En passant square
    if (pos.enpassant == Square::OFFSQ) {
        fen += " -";
    } else {
        fen += " " + SquareString[npos.enpassant];
    }

    // Part 5 -- Halfmove clock
    fen += " " + std::to_string(npos.halfmoves);

    // Part 6 -- Fullmove number
    fen += " " + std::to_string(npos.fullmoves);

    return fen;
}
