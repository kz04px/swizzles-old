#ifndef MCTS_UCT_NODE_HPP
#define MCTS_UCT_NODE_HPP

#include <cstdint>

struct Node {
    int visits_{0};
    float reward_{0.0};
    // bool terminal_{false};
    // bool fully_expanded_{false};
};

#endif
