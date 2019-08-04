#include "movegen.hpp"
#include "assert.hpp"
#include "move.hpp"
#include "position.hpp"

int movegen(const Position &pos, Move *movelist) {
    UCI_ASSERT(movelist);

    int num_moves = 0;
    num_moves += movegen_captures(pos, &movelist[0]);
    num_moves += movegen_noncaptures(pos, &movelist[num_moves]);

    UCI_ASSERT(num_moves >= 0);
    UCI_ASSERT(num_moves < MAX_MOVES);

    return num_moves;
}
