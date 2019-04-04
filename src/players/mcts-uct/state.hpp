#ifndef MCTS_UCT_STATE_HPP
#define MCTS_UCT_STATE_HPP

#include <vector>
#include "../../move.hpp"
#include "../../position.hpp"

struct State {
    State(const Position &pos, const Move &move);
    Move next();

    const Position pos_;
    const Move move_;
    std::vector<Move> moves_;
    int visits_;
    float reward_;
};

#endif
