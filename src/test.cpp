#include "test.hpp"
#include <iostream>
#include "fen.hpp"
#include "flip.hpp"
#include "perft.hpp"

bool test_fen() {
    const std::string fens[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b - - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e3 40 60",
    };
    for (const auto &fen : fens) {
        Position pos;
        set_fen(pos, fen);
        if (get_fen(pos) != fen) {
            return false;
        }
    }
    return true;
}

bool test_perft() {
    const std::pair<std::string, std::vector<std::uint64_t>> tests[] = {
        {"startpos", {20, 400, 8902}},
        {"r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", {25, 567, 14095}}};
    for (const auto &[fen, nodes] : tests) {
        Position pos;
        set_fen(pos, fen);

        for (unsigned int i = 0; i < nodes.size(); ++i) {
            if (nodes[i] != perft(pos, i + 1)) {
                return false;
            }
        }
    }
    return true;
}

bool test_flip() {
    const std::string fens[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b - - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
    };
    for (const auto &fen : fens) {
        Position pos;
        set_fen(pos, fen);
        Position npos = pos;
        flip(npos);
        flip(npos);
        if (npos != pos) {
            return false;
        }
    }
    return true;
}

void test() {
    std::cout << (test_fen() ? "Y" : "N") << " -- FEN parsing" << std::endl;
    std::cout << (test_perft() ? "Y" : "N") << " -- Perft" << std::endl;
    std::cout << (test_flip() ? "Y" : "N") << " -- Flip" << std::endl;
}
