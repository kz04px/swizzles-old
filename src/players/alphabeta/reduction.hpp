#ifndef SEARCH_ALPHABETA_REDUCTION_HPP
#define SEARCH_ALPHABETA_REDUCTION_HPP

#include "../../move.hpp"

int reduction(const bool pvnode,
              const int move_num,
              const int depth,
              const int in_check,
              const int move_type) {
    if (pvnode || move_num < 4 || depth < 3 || in_check ||
        move_type == MoveType::CAPTURE || move_type == MoveType::PROMO ||
        move_type == MoveType::PROMO_CAPTURE) {
        return 0;
    }

    return 1;
}

#endif
