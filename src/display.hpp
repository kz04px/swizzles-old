#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstdint>

struct Position;

void display(const Position &pos);
void display_u64(const std::uint64_t bb);

#endif
