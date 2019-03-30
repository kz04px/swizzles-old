#ifndef MAKEMOVE_HPP
#define MAKEMOVE_HPP

#include "move.hpp"
#include "position.hpp"

void make_move(Position &pos, const Move m);
bool make_move(Position &pos, const std::string &move_string);

#endif
