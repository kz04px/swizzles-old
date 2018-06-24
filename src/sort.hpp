#ifndef SORT_HPP
#define SORT_HPP

#include "position.hpp"
#include "move.hpp"

void sort(const Position &pos, Move *moves, const int num, const Move tt_move, const Move killer1, const Move killer2);
void sort_see(const Position &pos, Move *moves, int *scores, const int num);

#endif
