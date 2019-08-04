#include "state.hpp"
#include "../../assert.hpp"
#include "../../attacks.hpp"
#include "../../makemove.hpp"
#include "../../movegen.hpp"

State::State(const Position &pos, const Move &move)
    : pos_(pos), move_(move), moves_(), visits_(0), reward_(0.0) {
    UCI_ASSERT(legal_position(pos));

    // No moves possible if the game is already over
    if (is_fifty_moves(pos) || repetitions(pos) == 2) {
        return;
    }

    Move moves[MAX_MOVES];
    int num_moves = movegen(pos, moves);

    for (int i = 0; i < num_moves; ++i) {
        Position npos = pos;
        make_move(npos, moves[i]);
        const bool in_check = check(npos, Colour::THEM);
        if (!in_check) {
            moves_.push_back(moves[i]);
        }
    }

    UCI_ASSERT(moves_.size() <= MAX_MOVES);
}

Move State::next() {
    UCI_ASSERT(moves_.size() > 0);
    Move n = moves_.back();
    moves_.pop_back();
    return n;
}
