#ifndef MCTS_UCT_STACK_HPP
#define MCTS_UCT_STACK_HPP

#include <vector>
#include "../../move.hpp"
#include "../../position.hpp"

struct Stack {
    int ply;
    std::vector<Position> positions;
    std::vector<Move> moves;
};

#endif
