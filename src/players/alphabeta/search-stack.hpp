#ifndef SEARCH_STACK_HPP
#define SEARCH_STACK_HPP

#include "../../move.hpp"

struct SearchStack {
    int ply;
    bool nullmove;
    Move killer1;
    Move killer2;
};

#endif
