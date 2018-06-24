#include <cassert>
#include "eval.hpp"
#include "flip.hpp"
#include "phase.hpp"
#include "bitboards.hpp"
#include "pst.hpp"
#include "king-safety.hpp"
#include "attacks.hpp"
#include "mobility.hpp"
#include "other.hpp"

const int passed_pawn_value[8] = {0, 10, 10, 15, 25, 40, 60, 0};

int eval(const Position &pos)
{
    Position npos = pos;
    int score = 0;
    int mid_score = 0;
    int end_score = 0;

    for(int i = 0; i < 2; ++i)
    {
        score += 100 * popcountll(npos.colour[Colour::US] & npos.pieces[PieceType::PAWN]);
        score += 300 * popcountll(npos.colour[Colour::US] & npos.pieces[PieceType::KNIGHT]);
        score += 325 * popcountll(npos.colour[Colour::US] & npos.pieces[PieceType::BISHOP]);
        score += 500 * popcountll(npos.colour[Colour::US] & npos.pieces[PieceType::ROOK]);
        score += 900 * popcountll(npos.colour[Colour::US] & npos.pieces[PieceType::QUEEN]);

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
        score += safety(npos);

        // Piece mobility
        mid_score += piece_mobility(npos);

        // Knight pair
        if(popcountll(npos.colour[Colour::US] & npos.pieces[PieceType::KNIGHT]) > 1)
        {
            score += 15;
        }

        // Bishop pair
        if(popcountll(npos.colour[Colour::US] & npos.pieces[PieceType::BISHOP]) > 1)
        {
            score += 20;
        }

        uint64_t pawns = npos.pieces[PieceType::PAWN] & npos.colour[Colour::US];
        uint64_t pawns_attacking = pawn_attacks(npos, Colour::US);
        uint64_t pawns_attacking_them = pawn_attacks(npos, Colour::THEM);
        uint64_t pawn_holes = ~( pawns_attacking      |
                                (pawns_attacking<<8)  |
                                (pawns_attacking<<16) |
                                (pawns_attacking<<24) |
                                (pawns_attacking<<32) |
                                (pawns_attacking<<40) |
                                (pawns_attacking<<48));

        // Knight outposts
        score += -20*popcountll(U64_CENTER & npos.pieces[PieceType::KNIGHT] & npos.colour[Colour::THEM] & pawn_holes & pawns_attacking_them);

        // Pawn chains
        //score += 10*popcountll(pawns & pawns_attacking);

        while(pawns)
        {
            const int sq = lsbll(pawns);
            const int f = sq%8;
            const int r = sq/8;
            const uint64_t bb = 1ULL << sq;
            const uint64_t file = get_file(f);
            const uint64_t adj_files = get_adj_files(f);

            // Passed pawn
            if(is_passed_pawn(Colour::US, sq, npos.pieces[PieceType::PAWN] & npos.colour[Colour::THEM]) == true)
            {
                //mid_score += passed_pawn_value[r]/2;
                end_score += passed_pawn_value[r];

                // Defended passed pawn
                if(bb & pawns_attacking)
                {
                    end_score += passed_pawn_value[r]/2;
                }
            }
/*
            // Isolated pawns
            if(!(adj_files & pawns))
            {
                score -= 20;
            }
*/
            pawns &= pawns-1;
        }

/*
        // Blocked pawns
        uint64_t blockers = (pawns << 8) & npos.colour[Colour::US];
        score -= 10*popcountll(blockers);
*/

        // Rook on the 7th
        if(U64_RANK_7 & npos.pieces[PieceType::ROOK] & npos.colour[Colour::US])
        {
            score += 10;
        }

        for(int f = 0; f < 8; ++f)
        {
            const uint64_t file = get_file(f);
            const uint64_t adj_files = get_adj_files(f);

            // Open file bonuses
            if((npos.pieces[PieceType::PAWN] & file) == 0ULL)
            {
                // Rook & Queen bonus
                if((npos.colour[Colour::US] & (npos.pieces[PieceType::ROOK] | npos.pieces[PieceType::QUEEN]) & get_file(f)) != 0ULL)
                {
                    mid_score += 20;
                }
            }
        }

        score = -score;
        mid_score = -mid_score;
        end_score = -end_score;
        flip(npos);
    }

    // Turn bonus
    score += 10;

    int p = phase(pos);
    score += ((mid_score * (256 - p)) + (end_score * p)) / 256;

    return score;
}
