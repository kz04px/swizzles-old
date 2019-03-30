#include "sort.hpp"
#include <cassert>
#include "pst.hpp"
#include "see.hpp"
#include "types.hpp"

const int order[6] = {1, 2, 2, 3, 4, 5};

void sort(const Position &pos,
          Move *moves,
          const int num,
          const Move tt_move,
          const Move killer1,
          const Move killer2) {
    assert(moves);
    assert(num < 256);

    int scores[num] = {0};

    for (int i = 0; i < num; ++i) {
        if (moves[i] == tt_move) {
            scores[i] = 2000000;
        } else if (move_type(moves[i]) == MoveType::CAPTURE) {
            scores[i] = 1000000 + see_capture(pos, moves[i]);
            assert(scores[i] > 0);
            assert(scores[i] < 2000000);
        } else if (moves[i] == killer1) {
            scores[i] = 999999;
        } else if (moves[i] == killer2) {
            scores[i] = 999998;
        } else {
            scores[i] = 0;
            // scores[i] = see_quiet(pos, moves[i]);
            // scores[i] = PST[move_piece(moves[i])][0][move_to(moves[i])] -
            // PST[move_piece(moves[i])][0][move_from(moves[i])];
            assert(scores[i] < 999998);
        }

        // assert(scores[i] >= 0);
    }

    for (int a = 0; a < num - 1; ++a) {
        int idx = a;

        for (int b = a + 1; b < num; ++b) {
            if (scores[b] > scores[idx]) {
                idx = b;
            }
        }

        Move store = moves[idx];
        moves[idx] = moves[a];
        moves[a] = store;

        int store2 = scores[idx];
        scores[idx] = scores[a];
        scores[a] = store2;
    }

#ifndef NDEBUG
    for (int i = 0; i < num - 1; ++i) {
        assert(scores[i] >= scores[i + 1]);
    }
#endif
}

void sort_see(const Position &pos, Move *moves, int *scores, const int num) {
    assert(moves);
    assert(num < 256);

    for (int i = 0; i < num; ++i) {
        scores[i] = see_capture(pos, moves[i]);
    }

    for (int a = 0; a < num - 1; ++a) {
        int idx = a;

        for (int b = a + 1; b < num; ++b) {
            if (scores[b] > scores[idx]) {
                idx = b;
            }
        }

        Move store = moves[idx];
        moves[idx] = moves[a];
        moves[a] = store;

        int store2 = scores[idx];
        scores[idx] = scores[a];
        scores[a] = store2;
    }

#ifndef NDEBUG
    for (int i = 0; i < num - 1; ++i) {
        assert(scores[i] >= scores[i + 1]);
    }
#endif
}
