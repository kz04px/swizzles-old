#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "hashtable.hpp"
#include "position.hpp"

void search(const Position &pos,
            Hashtable &tt,
            bool &stop,
            int depth,
            int movetime,
            int nodes,
            bool infinite,
            int wtime,
            int btime,
            int winc,
            int binc,
            int movestogo);

#endif
