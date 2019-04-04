#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <cstdint>
#include "../hashtable.hpp"
#include "../position.hpp"
#include "../search-options.hpp"

namespace player {

// Play using alphabeta
void alphabeta(const Position &pos,
               Hashtable &tt,
               bool &stop,
               SearchOptions options);

// Play using MCTS-UCT
void mcts(const Position &pos,
          Hashtable &tt,
          bool &stop,
          SearchOptions options);

// Play a random move
void random(const Position &pos);

}  // namespace player

#endif
