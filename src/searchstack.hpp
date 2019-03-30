#ifndef SEARCHSTACK_HPP
#define SEARCHSTACK_HPP

#include "move.hpp"

struct SearchStack {
    int ply;
    bool nullmove;
    Move killer1;
    Move killer2;
};

#endif
