#ifndef TUNER_QSEARCH_HPP
#define TUNER_QSEARCH_HPP

#include <cassert>
#include <vector>
#include "../attacks.hpp"
#include "../makemove.hpp"
#include "../move.hpp"
#include "../movegen.hpp"
#include "../position.hpp"
#include "../see.hpp"
#include "../sort.hpp"
#include "../types.hpp"
#include "eval.hpp"

int tune_qsearch(const Position &pos,
                 int alpha,
                 int beta,
                 const std::vector<int> &values) {
    assert(beta > alpha);

    const int stand_pat = tune_eval(pos, values);

    if (stand_pat >= beta) {
        return beta;
    }

    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    Move moves[128];
    const int num_moves = movegen_captures(pos, moves);
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

        int score = -tune_qsearch(npos, -beta, -alpha, values);

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

#endif
