#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include <cstdint>
#include <ctime>
#include "../../hashtable.hpp"

struct SearchInfo {
    SearchInfo()
        : nodes(0ULL),
          leafnodes(0ULL),
          start(0),
          end(0),
          stop(nullptr),
          tt(nullptr),
          cutoffs{} {
    }

    std::uint64_t nodes;
    std::uint64_t leafnodes;
    int seldepth;
    clock_t start;
    clock_t end;
    bool *stop;
    Hashtable *tt;
    std::uint64_t cutoffs[MAX_MOVES];
};

#endif
