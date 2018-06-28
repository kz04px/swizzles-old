#ifndef MCTS_UCT_HPP_INCLUDED
#define MCTS_UCT_HPP_INCLUDED

#include "position.hpp"
#include "hashtable.hpp"

void mcts_uct(const Position &pos, Hashtable &tt, bool &stop, int movetime, int nodes, bool infinite, int wtime, int btime, int winc, int binc, int movestogo);

#endif
