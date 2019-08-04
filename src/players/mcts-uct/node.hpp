#ifndef MCTS_UCT_NODE_HPP
#define MCTS_UCT_NODE_HPP

#include <vector>
#include "state.hpp"

struct Position;
struct Move;

struct Node {
    Node(const Position &pos, const Move &move, Node *parent);
    ~Node();
    Node *expand();
    bool fully_expanded() const;
    bool terminal() const;

    State state_;
    Node *parent_;
    std::vector<Node> children_;
};

#endif
