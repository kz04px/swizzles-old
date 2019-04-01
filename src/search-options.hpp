#ifndef SEARCH_OPTIONS_HPP
#define SEARCH_OPTIONS_HPP

#include <cstdint>

enum SearchType
{
    Time = 0,
    Movetime,
    Depth,
    Nodes,
    Infinite
};

struct SearchOptions {
   public:
    SearchType type = SearchType::Movetime;
    int depth = 0;
    int movetime = 1000;
    uint64_t nodes = 0;
    int wtime = 0;
    int btime = 0;
    int winc = 0;
    int binc = 0;
    int movestogo = 30;
};

#endif
