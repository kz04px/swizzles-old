#include "movegen.hpp"
#include <cassert>

int movegen(const Position &pos, Move *movelist) {
    assert(movelist);

    int num_moves = 0;
    num_moves += movegen_captures(pos, &movelist[0]);
    num_moves += movegen_noncaptures(pos, &movelist[num_moves]);

    assert(num_moves >= 0);
    assert(num_moves < MAX_MOVES);

    return num_moves;
}
