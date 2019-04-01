#ifndef MCTS_UCT_HPP_INCLUDED
#define MCTS_UCT_HPP_INCLUDED

#include "hashtable.hpp"
#include "position.hpp"
#include "search-options.hpp"

void mcts_uct(const Position &pos,
              Hashtable &tt,
              bool &stop,
              SearchOptions options);

#endif
