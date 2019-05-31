#ifndef PERFT_HPP
#define PERFT_HPP

#include <cstdint>

struct Position;

uint64_t perft(const Position &pos, const int depth);

#endif
