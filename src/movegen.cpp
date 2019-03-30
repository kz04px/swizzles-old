#include "movegen.hpp"
#include <cassert>
#include <cstdint>
#include "attacks.hpp"
#include "bitboards.hpp"
#include "legal.hpp"
#include "move.hpp"
#include "other.hpp"
#include "position.hpp"
#include "types.hpp"

int movegen(const Position &pos, Move *movelist) {
    assert(movelist);

    int num_moves = 0;
    num_moves += movegen_captures(pos, &movelist[0]);
    num_moves += movegen_noncaptures(pos, &movelist[num_moves]);

    assert(num_moves >= 0);
    assert(num_moves <= 256);

    return num_moves;
}
