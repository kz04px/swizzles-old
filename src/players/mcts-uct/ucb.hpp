#ifndef MCTS_UCT_UCB_HPP
#define MCTS_UCT_UCB_HPP

#include <cassert>
#include <cmath>
#include "node.hpp"

float ucb(const Node *p, const Node *c, const float cp) {
    assert(p);
    assert(c);
    assert(p->visits_ > 0);
    assert(c->visits_ > 0);
    const float exploitation = c->reward_ / c->visits_;
    const float exploration = cp * sqrt(2.0 * log(p->visits_) / c->visits_);
    return exploitation + exploration;
}

#endif
