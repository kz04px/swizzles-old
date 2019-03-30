#ifndef FEN_HPP
#define FEN_HPP

#include <string>
#include "position.hpp"

bool set_fen(Position &pos, const std::string &fen);
std::string get_fen(const Position &pos);

#endif
