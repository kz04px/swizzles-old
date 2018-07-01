#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include "position.hpp"
#include "move.hpp"

int movegen(const Position &pos, Move *movelist);
int movegen_captures(const Position &pos, Move *movelist);
int movegen_noncaptures(const Position &pos, Move *movelist);

#endif
