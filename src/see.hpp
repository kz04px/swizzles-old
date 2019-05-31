#ifndef SEE_HPP_INCLUDED
#define SEE_HPP_INCLUDED

#include "move.hpp"

struct Position;

int see_capture(const Position &pos, const Move &move);
int see_quiet(const Position &pos, const Move &move);

#endif
