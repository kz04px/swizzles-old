#include "eval.hpp"
#include <cassert>
#include "attacks.hpp"
#include "bitboards.hpp"
#include "king-safety.hpp"
#include "mobility.hpp"
#include "other.hpp"
#include "phase.hpp"
#include "position.hpp"
#include "pst.hpp"

const int turn_value = 10;
const int open_file_value = 20;
const int rook_7th_value = 10;
// Piece values
const int piece_value[5] = {100, 300, 325, 500, 900};
// Piece pairs
const int bishop_pair_value = 20;
const int knight_pair_value = 15;
// Outposts
const int knight_outpost_value = 20;
const int bishop_outpost_value = 20;
const int rook_outpost_value = 20;
// Pawns
const int doubled_pawn_value = -10;
const int isolated_pawn_value = -20;
const int backward_pawn_value = -10;
const int pawn_chain_value = 10;
const int unreachable_pawn_value = 50;
// Pawns - passers
const int passer_rank_value[8] = {0, 10, 10, 15, 25, 40, 60, 0};

int king_distance(const int sq1, const int sq2) {
    assert(sq1 >= Square::A1);
    assert(sq1 <= Square::H8);
    assert(sq2 >= Square::A1);
    assert(sq2 <= Square::H8);

    const int dx = abs(sq_to_file(sq1) - sq_to_file(sq2));
    const int dy = abs(sq_to_rank(sq1) - sq_to_rank(sq2));
    return std::max(dx, dy);
}

int eval(const Position &pos) {
    Position npos = pos;
    int score = 0;
    int mid_score = 0;
    int end_score = 0;

    for (int i = 0; i < 2; ++i) {
        const bool our_turn = (i == 0);
        const uint64_t pawns =
            npos.pieces[PieceType::PAWN] & npos.colour[Colour::US];
        uint64_t copy = 0ULL;

        // Count pieces
        int counts[6] = {0};
        for (int p = 0; p < 5; ++p) {
            counts[p] = popcountll(npos.colour[Colour::US] & npos.pieces[p]);
        }

        // Material
        for (int p = 0; p < 5; ++p) {
            score += piece_value[p] * counts[p];
        }

        // Midgame piece square tables
        mid_score += score_pst(npos, 0, PieceType::PAWN);
        mid_score += score_pst(npos, 0, PieceType::KNIGHT);
        mid_score += score_pst(npos, 0, PieceType::BISHOP);
        mid_score += score_pst(npos, 0, PieceType::ROOK);
        mid_score += score_pst(npos, 0, PieceType::QUEEN);
        mid_score += score_pst(npos, 0, PieceType::KING);

        // Endgame piece square tables
        end_score += score_pst(npos, 1, PieceType::PAWN);
        end_score += score_pst(npos, 1, PieceType::KNIGHT);
        end_score += score_pst(npos, 1, PieceType::BISHOP);
        end_score += score_pst(npos, 1, PieceType::ROOK);
        end_score += score_pst(npos, 1, PieceType::QUEEN);
        end_score += score_pst(npos, 1, PieceType::KING);

        // King safety
        mid_score += safety(npos);

        // Piece mobility
        mid_score += piece_mobility(npos);

        // Knight pair
        if (counts[PieceType::KNIGHT] > 1) {
            score += knight_pair_value;
        }

        // Bishop pair
        if (counts[PieceType::BISHOP] > 1) {
            score += bishop_pair_value;
        }

        copy = pawns;
        while (copy) {
            const int sq = lsbll(copy);
            const int f = sq_to_file(sq);
            const int r = sq_to_rank(sq);
            const uint64_t bb = 1ULL << sq;
            const uint64_t file = get_file(f);
            const uint64_t adj_files = get_adj_files(f);

            // Isolated pawns
            if (!(adj_files & pawns)) {
                score += isolated_pawn_value;
            }

            // Doubled pawns
            if (popcountll(pawns & file) > 1) {
                score += doubled_pawn_value;
            }

            // Passed pawn
            if (is_passed_pawn(Colour::US,
                               sq,
                               npos.pieces[PieceType::PAWN] &
                                   npos.colour[Colour::THEM]) == true) {
                assert(0 < r && r < 7);
                assert(Square::A2 <= sq && sq <= Square::H7);
                assert(8 * (8 - r) < 64);
                assert(8 * (r + 1) < 64);

                const int promo_sq = Square::A8 + f;
                const uint64_t promo_bb = 1ULL << promo_sq;
                const uint64_t behind = file >> (8 * (8 - r));
                const uint64_t infront = file << (8 * (r + 1));

                assert(promo_sq >= Square::A8);
                assert(promo_sq <= Square::H8);
                assert(promo_bb & U64_RANK_8);
                assert(promo_bb & infront);
                assert(infront);
                assert(behind);
                assert(infront & promo_bb);
                assert((behind | infront | bb) == file);

                // Bonus for how advanced we are
                end_score += passer_rank_value[r];
            }

            copy &= copy - 1;
        }

        // Each file
        for (int f = 0; f < 8; ++f) {
            const uint64_t file = get_file(f);
            const uint64_t adj_files = get_adj_files(f);

            // Open file bonuses
            if ((npos.pieces[PieceType::PAWN] & file) == 0ULL) {
                // Rook & Queen bonus
                if (file & npos.colour[Colour::US] &
                    (npos.pieces[PieceType::ROOK] |
                     npos.pieces[PieceType::QUEEN])) {
                    mid_score += open_file_value;
                }
            }
        }

        score = -score;
        mid_score = -mid_score;
        end_score = -end_score;
        flip(npos);
    }

    // Turn bonus
    score += turn_value;

    int p = phase(pos);
    score += ((mid_score * (256 - p)) + (end_score * p)) / 256;

    assert(score >= -INF);
    assert(score <= INF);

    return score;
}
