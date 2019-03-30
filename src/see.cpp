#include "see.hpp"

int see(int sq,
        int side,
        int captured,
        uint64_t colours[2],
        uint64_t pieces[6]) {
    int value = 0;
    PieceType smallest_attacker = PieceType::NONE;

    uint64_t attackers = 0;

    // Pawns
    if ((attackers = magic_moves_pawn(1 - side, sq) & pieces[PieceType::PAWN] &
                     colours[side])) {
        smallest_attacker = PieceType::PAWN;
    }
    // Knights
    else if ((attackers = colours[side] & pieces[PieceType::KNIGHT] &
                          magic_moves_knight(sq))) {
        smallest_attacker = PieceType::KNIGHT;
    }
    // Bishops
    else if ((attackers =
                  colours[side] & pieces[PieceType::BISHOP] &
                  magic_moves_bishop(
                      colours[Colour::US] | colours[Colour::THEM], sq))) {
        smallest_attacker = PieceType::BISHOP;
    }
    // Rooks
    else if ((attackers =
                  colours[side] & pieces[PieceType::ROOK] &
                  magic_moves_rook(colours[Colour::US] | colours[Colour::THEM],
                                   sq))) {
        smallest_attacker = PieceType::ROOK;
    }
    // Queens
    else if ((attackers =
                  colours[side] & pieces[PieceType::QUEEN] &
                  (magic_moves_bishop(
                       colours[Colour::US] | colours[Colour::THEM], sq) |
                   magic_moves_rook(colours[Colour::US] | colours[Colour::THEM],
                                    sq)))) {
        smallest_attacker = PieceType::QUEEN;
    }
    // Kings
    else if ((attackers = colours[side] & pieces[PieceType::KING] &
                          magic_moves_king(sq))) {
        smallest_attacker = PieceType::KING;
    } else {
        // skip if the square isn't attacked anymore by this side
        return value;
    }

    int from_sq = lsbll(attackers);  //__builtin_ctzll
    uint64_t from_bb = 1ULL << from_sq;

    // Make move
    pieces[smallest_attacker] ^= from_bb;
    colours[side] ^= from_bb;

    value = piece_value[captured] -
            see(sq, 1 - side, smallest_attacker, colours, pieces);

    if (value < 0) {
        value = 0;
    }

    // Undo move
    pieces[smallest_attacker] ^= from_bb;
    colours[side] ^= from_bb;

    return value;
}

int see_capture(const Position &pos, const Move &move) {
    Position npos = pos;
    uint64_t from_bb = 1ULL << move_from(move);

    // Make move
    npos.pieces[move_piece(move)] ^= from_bb;
    npos.colour[npos.flipped] ^= from_bb;

    int value = piece_value[move_captured(move)] - see(move_to(move),
                                                       1 - npos.flipped,
                                                       move_piece(move),
                                                       npos.colour,
                                                       npos.pieces);

    return value;
}

int see_quiet(const Position &pos, const Move &move) {
    Position npos = pos;
    uint64_t from_bb = 1ULL << move_from(move);

    // Make move
    npos.pieces[move_piece(move)] ^= from_bb;
    npos.colour[npos.flipped] ^= from_bb;

    int value = 0 - see(move_to(move),
                        1 - npos.flipped,
                        move_piece(move),
                        npos.colour,
                        npos.pieces);

    return value;
}
