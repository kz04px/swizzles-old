#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <cstdint>
#include "hashtable.hpp"
#include "position.hpp"

enum SearchType
{
    Time,
    Movetime,
    Depth,
    Nodes
};

struct SearchOptions {
   public:
    SearchType type = SearchType::Depth;
    int depth = 1;
    int movetime = 0;
    uint64_t nodes = 0;
    int wtime = 0;
    int btime = 0;
    int winc = 0;
    int binc = 0;
    int movestogo = 30;
};

void search(const Position &pos,
            Hashtable &tt,
            bool &stop,
            SearchOptions &options);

#endif
