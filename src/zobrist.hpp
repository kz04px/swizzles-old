#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <cstdint>
#include "position.hpp"

void zobrist_init();
std::uint64_t calculate_hash(const Position &pos);

#endif
