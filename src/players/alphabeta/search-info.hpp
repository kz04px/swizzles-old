#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include <chrono>
#include <cstdint>
#include <ctime>
#include "../../hashtable.hpp"

struct SearchInfo {
    SearchInfo()
        : nodes(0ULL),
          leafnodes(0ULL),
          stop(nullptr),
          tt(nullptr),
          hash_hits(0ULL),
          hash_collisions(0ULL),
          cutoffs{} {
    }

    std::uint64_t nodes;
    std::uint64_t leafnodes;
    int seldepth;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    bool *stop;
    Hashtable *tt;
    std::uint64_t hash_hits;
    std::uint64_t hash_collisions;
    std::uint64_t cutoffs[MAX_MOVES];
};

#endif
