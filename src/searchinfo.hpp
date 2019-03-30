#ifndef SEARCHINFO_HPP
#define SEARCHINFO_HPP

#include <cstdint>
#include "hashtable.hpp"

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
    clock_t start;
    clock_t end;
    bool *stop;
    Hashtable *tt;
#ifndef NDEBUG
    std::uint64_t cutoffs[256];
#endif
    /*
    int depth;
    int selDepth;
    */
};

#endif
