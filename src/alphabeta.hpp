#ifndef ALPHABETA_HPP
#define ALPHABETA_HPP

#include "position.hpp"
#include "searchinfo.hpp"
#include "searchstack.hpp"
#include "pv.hpp"

int alphabeta(const Position &pos, SearchInfo &info, SearchStack *ss, PV &pv, int alpha, int beta, int depth);

#endif
