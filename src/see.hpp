#ifndef SEE_HPP_INCLUDED
#define SEE_HPP_INCLUDED

#include <cassert>
#include "bitboards.hpp"
#include "eval.hpp"
#include "move.hpp"
#include "other.hpp"
#include "position.hpp"
#include "types.hpp"

int see_capture(const Position &pos, const Move &move);
int see_quiet(const Position &pos, const Move &move);

#endif
