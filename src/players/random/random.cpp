#include <iostream>
#include "../../attacks.hpp"
#include "../../makemove.hpp"
#include "../../movegen.hpp"
#include "../../position.hpp"
#include "../all.hpp"

namespace player {

void random(const Position &pos) {
    Move moves[MAX_MOVES];
    int num_moves = movegen(pos, moves);

    // Filter illegal moves
    for (int i = num_moves - 1; i >= 0; --i) {
        Position npos = pos;
        make_move(npos, moves[i]);
        const bool in_check = check(npos, Colour::THEM);
        if (in_check) {
            moves[i] = moves[num_moves - 1];
            num_moves--;
        }
    }

    // Return a null move if we have no legal moves
    if (num_moves == 0) {
        std::cout << "bestmove 0000" << std::endl;
    }

    // Return random move from legal moves
    const int n = rand() % num_moves;
    std::cout << "bestmove " << move_uci(moves[n], pos.flipped) << std::endl;
}

}  // namespace player
