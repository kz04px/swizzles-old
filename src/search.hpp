#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <cstdint>
#include "hashtable.hpp"
#include "move.hpp"
#include "position.hpp"
#include "pv.hpp"
#include "search-options.hpp"
#include "types.hpp"

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
    std::uint64_t cutoffs[MAX_MOVES];
};

struct SearchStack {
    int ply;
    bool nullmove;
    Move killer1;
    Move killer2;
};

void search(const Position &pos,
            Hashtable &tt,
            bool &stop,
            SearchOptions options);
int alphabeta(const Position &pos,
              SearchInfo &info,
              SearchStack *ss,
              PV &pv,
              int alpha,
              int beta,
              int depth);
int qsearch(const Position &pos,
            SearchInfo &info,
            SearchStack *ss,
            int alpha,
            int beta);
uint64_t perft(const Position &pos, const int depth);

#endif
