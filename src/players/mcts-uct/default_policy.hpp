#ifndef MCTS_UCT_DEFAULT_POLICY_HPP
#define MCTS_UCT_DEFAULT_POLICY_HPP

#include <cmath>
#include "../../eval.hpp"
#include "../../position.hpp"

#define SIGMOID(x) (1.0 / (1.0 + exp(-x)))

float default_policy(const Position &pos) {
    float score = 0.0;
    bool legal_moves = false;

    Move moves[MAX_MOVES];
    int num_moves = movegen(pos, moves);
    for (int i = 0; i < num_moves; ++i) {
        Position npos = pos;
        make_move(npos, moves[i]);
        if (check(npos, Colour::THEM)) {
            continue;
        }
        legal_moves = true;
        break;
    }

    // Game over
    if (!legal_moves) {
        const bool in_check = check(pos, Colour::US);
        if (in_check) {
            score = -INF;
        } else {
            score = 0;
        }
    }
    // Draw score
    else if (is_fifty_moves(pos) || repetitions(pos) == 2) {
        score = 0.0;
    }
    // Eval
    else {
        score = 0.001 * eval(pos);
    }

    return 1.0 - SIGMOID(score);
}

#endif
