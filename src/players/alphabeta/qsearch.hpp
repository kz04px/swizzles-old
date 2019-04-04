#ifndef QSEARCH_HPP
#define QSEARCH_HPP

#include "../../position.hpp"
#include "search-info.hpp"
#include "search-stack.hpp"

int qsearch(const Position &pos,
            SearchInfo &info,
            SearchStack *ss,
            int alpha,
            int beta);

#endif
