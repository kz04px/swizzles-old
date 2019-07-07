#ifndef QSEARCH_HPP
#define QSEARCH_HPP

struct Position;
struct SearchInfo;
struct SearchStack;

int qsearch(const Position &pos,
            SearchInfo &info,
            SearchStack *ss,
            int alpha,
            int beta);

#endif
