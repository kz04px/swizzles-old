#ifndef QSEARCH_HPP
#define QSEARCH_HPP

#include "position.hpp"
#include "searchinfo.hpp"
#include "searchstack.hpp"

int qsearch(const Position &pos,
            SearchInfo &info,
            SearchStack *ss,
            int alpha,
            int beta);

#endif
