#ifndef MCTS_UCT_NODE_HPP
#define MCTS_UCT_NODE_HPP

#include <vector>
#include "../../position.hpp"
#include "state.hpp"

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
