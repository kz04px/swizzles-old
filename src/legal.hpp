#ifndef LEGAL_HPP
#define LEGAL_HPP

#include <string>
#include "position.hpp"
#include "move.hpp"

bool legal_move(const Position &pos, const Move &move);
bool legal_move(const Position &pos, const std::string &move);

#endif
