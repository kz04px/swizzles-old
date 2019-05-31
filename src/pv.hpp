#ifndef PV_HPP
#define PV_HPP

#include <cassert>
#include "makemove.hpp"
#include "move.hpp"
#include "position.hpp"

struct PV {
    PV() : length(0), moves{} {
    }

    bool legal(const Position &pos) const {
        Position npos = pos;
        for (int i = 0; i < length; ++i) {
            if (legal_move(npos, moves[i]) == false) {
                return false;
            }

            make_move(npos, moves[i]);
        }
        return true;
    }

    std::string string(bool flipped) const {
        std::string str = "";
        for (int n = 0; n < length; ++n) {
            if (n > 0) {
                str += " ";
            }
            str += move_uci(moves[n], flipped);
            flipped = !flipped;
        }
        return str;
    }

    void add(const Move m) {
        moves[length] = m;
        length++;
        assert(length < 64);
    }

    int length;
    Move moves[64];
};

#endif
