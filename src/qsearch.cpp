#include "attacks.hpp"
#include "eval.hpp"
#include "makemove.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "search.hpp"
#include "see.hpp"
#include "sort.hpp"
#include "types.hpp"

int qsearch(const Position &pos,
            SearchInfo &info,
            SearchStack *ss,
            int alpha,
            int beta) {
    assert(ss);
    assert(beta > alpha);

    if (ss->ply >= 256) {
        return eval(pos);
    }

    if (*info.stop == true || clock() > info.end) {
        return 0;
    }

    int stand_pat = eval(pos);

    if (stand_pat >= beta) {
        return beta;
    }

    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    Move moves[128];
    int num_moves = movegen_captures(pos, moves);
    if (num_moves == 0) {
        return alpha;
    }
    // int scores[128];
    // sort_see(pos, moves, scores, num_moves);

    for (int i = 0; i < num_moves; ++i) {
        // if(scores[i] < -50) {break;}
        if (see_capture(pos, moves[i]) < -50) {
            continue;
        }

        Position npos = pos;
        make_move(npos, moves[i]);
        if (check(npos, Colour::THEM) == true) {
            continue;
        }

        info.nodes++;

        int score = -qsearch(npos, info, ss + 1, -beta, -alpha);

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}
