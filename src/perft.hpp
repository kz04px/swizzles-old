#ifndef PERFT_HPP
#define PERFT_HPP

#include <cstdint>
#include "position.hpp"

uint64_t perft(const Position &pos, const int depth);

#endif
