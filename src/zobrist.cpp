#include "zobrist.hpp"
#include "other.hpp"
#include "position.hpp"
#include "types.hpp"

namespace zobrist {

std::uint64_t calculate_hash(const Position &pos) {
    std::uint64_t hash = 0ULL;

    // Pieces
    for (int i = PieceType::PAWN; i <= PieceType::KING; ++i) {
        std::uint64_t copy = pos.pieces[i] & pos.colour[Colour::US];
        while (copy) {
            const int sq = lsbll(copy);

            hash ^= keys::piece[sq][i][Colour::US];

            copy &= copy - 1;
        }

        copy = pos.pieces[i] & pos.colour[Colour::THEM];
        while (copy) {
            const int sq = lsbll(copy);

            hash ^= keys::piece[sq][i][Colour::THEM];

            copy &= copy - 1;
        }
    }

    // En passant
    if (pos.enpassant != Square::A1) {
        int file = pos.enpassant % 8;
        hash ^= keys::enpassant[file];
    }

    // Castling
    if (pos.castling[Castling::usKSC] == true) {
        hash ^= keys::castling[Castling::usKSC];
    }
    if (pos.castling[Castling::usQSC] == true) {
        hash ^= keys::castling[Castling::usQSC];
    }
    if (pos.castling[Castling::themKSC] == true) {
        hash ^= keys::castling[Castling::themKSC];
    }
    if (pos.castling[Castling::themQSC] == true) {
        hash ^= keys::castling[Castling::themQSC];
    }

    // Flipped
    if (pos.flipped == true) {
        hash ^= keys::flipped;
    }

    return hash;
}

}  // namespace zobrist
