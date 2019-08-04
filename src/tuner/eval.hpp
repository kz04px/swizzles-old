#ifndef TUNER_EVAL_HPP
#define TUNER_EVAL_HPP

#include <vector>
#include "../assert.hpp"
#include "../attacks.hpp"
#include "../bitboards.hpp"
#include "../king-safety.hpp"
#include "../mobility.hpp"
#include "../other.hpp"
#include "../phase.hpp"
#include "../position.hpp"
#include "../pst.hpp"

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
const int passer_file_value[8] = {10, 5, 0, 0, 0, 0, 5, 10};

int tune_king_distance(const int sq1, const int sq2) {
    UCI_ASSERT(sq1 >= Square::A1);
    UCI_ASSERT(sq1 <= Square::H8);
    UCI_ASSERT(sq2 >= Square::A1);
    UCI_ASSERT(sq2 <= Square::H8);

    const int dx = abs(sq_to_file(sq1) - sq_to_file(sq2));
    const int dy = abs(sq_to_rank(sq1) - sq_to_rank(sq2));
    return std::max(dx, dy);
}

int tune_eval(const Position &pos, const std::vector<int> &tuned) {
    Position npos = pos;
    int score = 0;
    int mid_score = 0;
    int end_score = 0;

    for (int i = 0; i < 2; ++i) {
        const bool our_turn = (i == 0);
        const int their_king_sq =
            lsbll(npos.pieces[PieceType::KING] & npos.colour[Colour::THEM]);
        const uint64_t pawns_attacking = pawn_attacks(npos, Colour::US);
        const uint64_t pawns_attacking_them = pawn_attacks(npos, Colour::THEM);
        const uint64_t pawns =
            npos.pieces[PieceType::PAWN] & npos.colour[Colour::US];
        const uint64_t pawn_holes = ~(
            pawns_attacking | (pawns_attacking << 8) | (pawns_attacking << 16) |
            (pawns_attacking << 24) | (pawns_attacking << 32) |
            (pawns_attacking << 40) | (pawns_attacking << 48));
        const uint64_t outposts =
            U64_CENTER & pawn_holes & pawns_attacking_them;
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

        // Knight outposts
        score += -knight_outpost_value *
                 popcountll(outposts & npos.pieces[PieceType::KNIGHT] &
                            npos.colour[Colour::THEM]);

        /*
        // Pawn chains
        const uint64_t left = (pawns >> 9) & (~U64_FILE_H);
        const uint64_t right = (pawns >> 7) & (~U64_FILE_A);
        score += pawn_chain_value * __builtin_popcountll(left & pawns);
        score += pawn_chain_value * __builtin_popcountll(right & pawns);
        */

        /*
        // Passed pawns
        const uint64_t passers = passed_pawns(
            pawns, npos.pieces[PieceType::PAWN] &
            npos.colour[Colour::THEM]); copy = passers; while (copy) { const int
            sq = lsbll(copy); const int f = sq_to_file(sq); const uint64_t
            adj_files = get_adj_files(f);

            // Connected passers
            if (adj_files & passers) {
                end_score += 10;
            }

            copy &= copy - 1;
        }
        */

        // Pawns
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
                UCI_ASSERT(0 < r && r < 7);
                UCI_ASSERT(Square::A2 <= sq && sq <= Square::H7);
                UCI_ASSERT(8 * (8 - r) < 64);
                UCI_ASSERT(8 * (r + 1) < 64);

                const int promo_sq = Square::A8 + f;
                const uint64_t promo_bb = 1ULL << promo_sq;
                const uint64_t behind = file >> (8 * (8 - r));
                const uint64_t infront = file << (8 * (r + 1));

                UCI_ASSERT(promo_sq >= Square::A8);
                UCI_ASSERT(promo_sq <= Square::H8);
                UCI_ASSERT(promo_bb & U64_RANK_8);
                UCI_ASSERT(promo_bb & infront);
                UCI_ASSERT(infront);
                UCI_ASSERT(behind);
                UCI_ASSERT(infront & promo_bb);
                UCI_ASSERT((behind | infront | bb) == file);

                /*
                // Enemy piece behind
                if (behind & npos.colour[Colour::THEM]) {
                    end_score -= 10;
                    // end_score -= passer_rank_value[r] >> 2;
                }
                */

                // Enemy piece on promotion square
                if (promo_bb & npos.colour[Colour::THEM]) {
                    end_score -= passer_rank_value[r] >> 2;
                }

                /*
                // Enemy pawn behind
                if (behind & npos.colour[Colour::THEM] &
                    npos.pieces[PieceType::PAWN]) {
                    end_score -= 10;
                    // end_score -= passer_rank_value[r] >> 2;
                }
                */

                /*
                // Enemy knight defending promotion square
                if (magic_moves_knight(promo_sq) & npos.colour[Colour::THEM] &
                    npos.pieces[PieceType::KNIGHT]) {
                    // end_score -= 10;
                    end_score -= passer_rank_value[r] >> 2;
                }
                */

                /*
                // Enemy pieces blocking
                if (infront & npos.colour[Colour::THEM]) {
                    end_score -= passer_rank_value[r] >> 2;
                }
                */

                /*
                // Enemy rook on the 8th rank
                if (U64_RANK_8 & npos.colour[Colour::THEM] &
                    npos.pieces[PieceType::ROOK]) {
                    end_score -= 10;
                }
                */

                // Bonus for how advanced we are
                end_score += passer_rank_value[r];

                /*
                // Bonus for file
                end_score += passer_file_value[f];
                */

                /*
                // Bonus for rooks behind
                if (behind & npos.pieces[PieceType::ROOK] &
                    npos.colour[Colour::US]) {
                    end_score += passer_rank_value[r];
                }
                */

                /*
                // Bonus for a bishop of the right colour
                if (square_colour[f % 2] & npos.pieces[PieceType::BISHOP] &
                    npos.colour[Colour::US]) {
                    end_score += 25;
                }
                */

                // Bonus when the enemy king can't reach us in time to promote
                if (tune_king_distance(promo_sq, their_king_sq) -
                        (our_turn ? 0 : 1) >
                    std::min(5, 7 - r)) {
                    end_score += unreachable_pawn_value;
                }

                // Defended passed pawn
                if (bb & pawns_attacking) {
                    score += passer_rank_value[r] / 2;
                }
            }

            copy &= copy - 1;
        }

        // Rook on the 7th
        if (U64_RANK_7 & npos.pieces[PieceType::ROOK] &
            npos.colour[Colour::US]) {
            score += rook_7th_value;
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

    UCI_ASSERT(score >= -INF);
    UCI_ASSERT(score <= INF);

    return score;
}

#endif
