#include "sort.hpp"
#include <cassert>
#include "position.hpp"
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
    assert(num < MAX_MOVES);

    int scores[num];

    for (int i = 0; i < num; ++i) {
        if (moves[i] == tt_move) {
            scores[i] = 2000000;
        } else if (moves[i].type() == MoveType::CAPTURE) {
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
            // scores[i] = PST[move.piece(moves[i])][0][move.to(moves[i])] -
            // PST[move.piece(moves[i])][0][move.from(moves[i])];
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

        std::swap(moves[idx], moves[a]);
        std::swap(scores[idx], scores[a]);
    }

#ifndef NDEBUG
    for (int i = 0; i < num - 1; ++i) {
        assert(scores[i] >= scores[i + 1]);
    }
#endif
}

void sort_see(const Position &pos, Move *moves, int *scores, const int num) {
    assert(moves);
    assert(scores);
    assert(num < MAX_MOVES);

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

        std::swap(moves[idx], moves[a]);
        std::swap(scores[idx], scores[a]);
    }

#ifndef NDEBUG
    for (int i = 0; i < num - 1; ++i) {
        assert(scores[i] >= scores[i + 1]);
    }
#endif
}
