#include "mobility.hpp"
#include "bitboards.hpp"
#include "other.hpp"

int piece_mobility(const Position &pos) {
    uint64_t moves = 0;
    uint64_t copy = 0;
    uint64_t allowed = ~pos.colour[Colour::US];
    int from = 0;

    // Knights
    copy = pos.pieces[PieceType::KNIGHT] & pos.colour[Colour::US];
    while (copy) {
        from = lsbll(copy);
        moves |= magic_moves_knight(from) & allowed;

        copy &= copy - 1;
    }

    // Bishops & Queens
    copy = (pos.pieces[PieceType::BISHOP] | pos.pieces[PieceType::QUEEN]) &
           pos.colour[Colour::US];
    while (copy) {
        from = lsbll(copy);
        moves |=
            magic_moves_bishop(
                (pos.colour[Colour::US] | pos.colour[Colour::THEM]), from) &
            allowed;

        copy &= copy - 1;
    }

    // Rooks & Queens
    copy = (pos.pieces[PieceType::ROOK] | pos.pieces[PieceType::QUEEN]) &
           pos.colour[Colour::US];
    while (copy) {
        from = lsbll(copy);
        moves |=
            magic_moves_rook(
                (pos.colour[Colour::US] | pos.colour[Colour::THEM]), from) &
            allowed;

        copy &= copy - 1;
    }

    int count = popcountll(moves);

    return 4 * count;
}
