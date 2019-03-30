#include "zobrist.hpp"
#include <random>
#include "other.hpp"
#include "types.hpp"

std::uint64_t keys_flipped;
std::uint64_t keys_castling[4];
std::uint64_t keys_enpassant[8];
std::uint64_t keys_piece[64][6][2];

void zobrist_init() {
    // Initialise
    // std::random_device rd;
    // std::mt19937_64 gen(rd());
    std::mt19937_64 gen(0x480ec5cb);

    // Define the number generator type
    std::uniform_int_distribution<uint64_t> dis;

    keys_flipped = dis(gen);

    for (int i = 0; i < 4; ++i) {
        keys_castling[i] = dis(gen);
    }

    for (int i = 0; i < 8; ++i) {
        keys_enpassant[i] = dis(gen);
    }

    for (int sq = 0; sq < 64; ++sq) {
        for (int piece = 0; piece < 6; ++piece) {
            keys_piece[sq][piece][Colour::US] = dis(gen);
            keys_piece[sq][piece][Colour::THEM] = dis(gen);
        }
    }
}

std::uint64_t calculate_hash(const Position &pos) {
    std::uint64_t hash = 0ULL;

    // Pieces
    for (int i = PieceType::PAWN; i <= PieceType::KING; ++i) {
        std::uint64_t copy = pos.pieces[i] & pos.colour[Colour::US];
        while (copy) {
            const int sq = lsbll(copy);

            hash ^= keys_piece[sq][i][Colour::US];

            copy &= copy - 1;
        }

        copy = pos.pieces[i] & pos.colour[Colour::THEM];
        while (copy) {
            const int sq = lsbll(copy);

            hash ^= keys_piece[sq][i][Colour::THEM];

            copy &= copy - 1;
        }
    }

    // En passant
    if (pos.enpassant != Square::OFFSQ) {
        int file = pos.enpassant % 8;
        hash ^= keys_enpassant[file];
    }

    // Castling
    if (pos.castling[Castling::usKSC] == true) {
        hash ^= keys_castling[Castling::usKSC];
    }
    if (pos.castling[Castling::usQSC] == true) {
        hash ^= keys_castling[Castling::usQSC];
    }
    if (pos.castling[Castling::themKSC] == true) {
        hash ^= keys_castling[Castling::themKSC];
    }
    if (pos.castling[Castling::themQSC] == true) {
        hash ^= keys_castling[Castling::themQSC];
    }

    // Flipped
    if (pos.flipped == true) {
        hash ^= keys_flipped;
    }

    return hash;
}
