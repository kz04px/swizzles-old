#ifndef SEE_HPP_INCLUDED
#define SEE_HPP_INCLUDED

#include <cassert>
#include "position.hpp"
#include "move.hpp"
#include "bitboards.hpp"
#include "types.hpp"
#include "eval.hpp"
#include "other.hpp"

int see_capture(const Position &pos, const Move &move);
int see_quiet(const Position &pos, const Move &move);

#endif
