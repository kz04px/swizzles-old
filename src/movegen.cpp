#include <cstdint>
#include <cassert>
#include "position.hpp"
#include "bitboards.hpp"
#include "types.hpp"
#include "move.hpp"
#include "attacks.hpp"
#include "movegen.hpp"
#include "legal.hpp"
#include "other.hpp"

int movegen(const Position &pos, Move *movelist)
{
    assert(movelist != NULL);

    int num_moves = 0;
    num_moves += movegen_captures(pos, &movelist[0]);
    num_moves += movegen_noncaptures(pos, &movelist[num_moves]);

    assert(num_moves >= 0);
    assert(num_moves <= 256);

    return num_moves;
}
