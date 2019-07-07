#include "phase.hpp"
#include "other.hpp"
#include "position.hpp"
#include "types.hpp"

int phase(const Position &pos) {
    const int knight_phase = 1;
    const int bishop_phase = 1;
    const int rook_phase = 2;
    const int queen_phase = 4;
    const int total_phase =
        knight_phase * 4 + bishop_phase * 4 + rook_phase * 4 + queen_phase * 2;

    int phase = total_phase;

    phase -= knight_phase * popcountll(pos.pieces[PieceType::KNIGHT]);
    phase -= bishop_phase * popcountll(pos.pieces[PieceType::BISHOP]);
    phase -= rook_phase * popcountll(pos.pieces[PieceType::ROOK]);
    phase -= queen_phase * popcountll(pos.pieces[PieceType::QUEEN]);

    phase = (phase * 256 + (total_phase / 2)) / total_phase;

    return phase;
}
