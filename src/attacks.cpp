#include "attacks.hpp"
#include "bitboards.hpp"
#include "other.hpp"
#include "position.hpp"

uint64_t squares_attacked(const Position &pos, const Colour side) {
    uint64_t attacks = 0ULL;

    attacks |= pawn_attacks(pos, side);
    attacks |= knight_attacks(pos, side);
    attacks |= bishop_attacks(pos, side);
    attacks |= rook_attacks(pos, side);
    attacks |= queen_attacks(pos, side);
    attacks |= king_attacks(pos, side);

    return attacks;
}

uint64_t attackers(const Position &pos, const Square sq, const Colour side) {
    uint64_t att = 0ULL;
    att ^= magic_moves_pawn(!side, sq) & pos.pieces[PieceType::PAWN] &
           pos.colour[side];
    att ^= magic_moves_knight(sq) & pos.pieces[PieceType::KNIGHT] &
           pos.colour[side];
    att ^= magic_moves_bishop(pos.colour[US] | pos.colour[THEM], sq) &
           (pos.pieces[PieceType::BISHOP] | pos.pieces[PieceType::QUEEN]) &
           pos.colour[side];
    att ^= magic_moves_rook(pos.colour[US] | pos.colour[THEM], sq) &
           (pos.pieces[PieceType::ROOK] | pos.pieces[PieceType::QUEEN]) &
           pos.colour[side];
    att ^=
        magic_moves_king(sq) & pos.pieces[PieceType::KING] & pos.colour[side];
    return att;
}

int num_attackers(const Position &pos, const Square sq, const Colour side) {
    return popcountll(attackers(pos, sq, side));
}

bool attacked(const Position &pos, const Square sq, const Colour side) {
    if (magic_moves_pawn(!side, sq) & pos.pieces[PieceType::PAWN] &
        pos.colour[side]) {
        return true;
    }
    if (magic_moves_knight(sq) & pos.pieces[PieceType::KNIGHT] &
        pos.colour[side]) {
        return true;
    }
    if (magic_moves_bishop(pos.colour[US] | pos.colour[THEM], sq) &
        (pos.pieces[PieceType::BISHOP] | pos.pieces[PieceType::QUEEN]) &
        pos.colour[side]) {
        return true;
    }
    if (magic_moves_rook(pos.colour[US] | pos.colour[THEM], sq) &
        (pos.pieces[PieceType::ROOK] | pos.pieces[PieceType::QUEEN]) &
        pos.colour[side]) {
        return true;
    }
    if (magic_moves_king(sq) & pos.pieces[PieceType::KING] & pos.colour[side]) {
        return true;
    }

    return false;
}

bool check(const Position &pos, const Colour side) {
    Square sq = Square(lsbll(pos.pieces[PieceType::KING] & pos.colour[side]));
    return attacked(pos, sq, Colour(!side));
}

uint64_t pawn_attacks(const Position &pos, const Colour side) {
    uint64_t pawns = pos.pieces[PieceType::PAWN] & pos.colour[side];
    if (side == Colour::US) {
        return ((pawns << 9) & ~U64_FILE_A) | ((pawns << 7) & ~U64_FILE_H);
    } else {
        return ((pawns >> 9) & ~U64_FILE_H) | ((pawns >> 7) & ~U64_FILE_A);
    }
}

uint64_t knight_attacks(const Position &pos, const Colour side) {
    uint64_t knights = pos.pieces[PieceType::KNIGHT] & pos.colour[side];

    uint64_t attacks = (knights << 17) & (~U64_FILE_A);       // Up 2 right 1
    attacks |= (knights << 15) & (~U64_FILE_H);               // Up 2 left 1
    attacks |= (knights >> 17) & (~U64_FILE_H);               // Down 2 left 1
    attacks |= (knights >> 15) & (~U64_FILE_A);               // Down 2 right 1
    attacks |= (knights << 10) & ~(U64_FILE_B | U64_FILE_A);  // Right 2 up 1
    attacks |= (knights >> 6) & ~(U64_FILE_B | U64_FILE_A);   // Right 2 down 1
    attacks |= (knights << 6) & ~(U64_FILE_H | U64_FILE_G);   // Left 2 up 1
    attacks |= (knights >> 10) & ~(U64_FILE_H | U64_FILE_G);  // Left 2 down 1

    return attacks;
}

uint64_t bishop_attacks(const Position &pos, const Colour side) {
    uint64_t bishops = pos.pieces[PieceType::BISHOP] & pos.colour[side];
    uint64_t attacks = 0ULL;

    while (bishops) {
        Square from = Square(lsbll(bishops));
        attacks |=
            magic_moves_bishop((pos.colour[US] | pos.colour[THEM]), from);
        bishops &= bishops - 1;
    }

    return attacks;
}

uint64_t rook_attacks(const Position &pos, const Colour side) {
    uint64_t rooks = pos.pieces[PieceType::ROOK] & pos.colour[side];
    uint64_t attacks = 0ULL;

    while (rooks) {
        Square from = Square(lsbll(rooks));
        attacks |= magic_moves_rook((pos.colour[US] | pos.colour[THEM]), from);
        rooks &= rooks - 1;
    }

    return attacks;
}

uint64_t queen_attacks(const Position &pos, const Colour side) {
    uint64_t queens = pos.pieces[PieceType::QUEEN] & pos.colour[side];
    uint64_t attacks = 0ULL;

    while (queens) {
        Square from = Square(lsbll(queens));
        attacks |=
            magic_moves_bishop((pos.colour[US] | pos.colour[THEM]), from);
        attacks |= magic_moves_rook((pos.colour[US] | pos.colour[THEM]), from);
        queens &= queens - 1;
    }

    return attacks;
}

uint64_t king_attacks(const Position &pos, const Colour side) {
    Square from = Square(lsbll(pos.pieces[PieceType::KING] & pos.colour[side]));
    return magic_moves_king(from);
}
