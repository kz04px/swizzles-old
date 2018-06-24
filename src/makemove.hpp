#ifndef MAKEMOVE_HPP
#define MAKEMOVE_HPP

#include "position.hpp"
#include "move.hpp"

void make_move(Position &pos, const Move m);
bool make_move(Position &pos, const std::string &move_string);

#endif
