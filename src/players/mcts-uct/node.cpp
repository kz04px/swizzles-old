#include "node.hpp"
#include "../../assert.hpp"
#include "../../makemove.hpp"
#include "../../move.hpp"
#include "../../position.hpp"

Node::Node(const Position &pos, const Move &move, Node *parent)
    : state_(pos, move), parent_(parent) {
}

Node::~Node() {
}

Node *Node::expand() {
    UCI_ASSERT(!terminal());
    UCI_ASSERT(!fully_expanded());

    Move move = state_.next();
    Position npos = state_.pos_;
    UCI_ASSERT(legal_move(state_.pos_, move));
    make_move(npos, move);
    Node child(npos, move, this);
    children_.push_back(child);
    return &children_.back();
}

bool Node::fully_expanded() const {
    return state_.moves_.size() == 0;
}

bool Node::terminal() const {
    return children_.size() == 0 && fully_expanded();
}
