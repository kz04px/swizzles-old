#include "test.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "fen.hpp"
#include "makemove.hpp"
#include "protocols.hpp"
#include "search.hpp"
#include "zobrist.hpp"

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
        {"r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", {25, 567, 14095}},
    };
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

bool test_hash() {
    const std::string fens[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b - - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1",
    };

    std::vector<uint64_t> hashes;

    for (const auto &fen : fens) {
        Position pos;
        set_fen(pos, fen);
        hashes.push_back(calculate_hash(pos));
    }

    for (unsigned int a = 0; a < hashes.size() - 1; ++a) {
        for (unsigned int b = a + 1; b < hashes.size(); ++b) {
            if (hashes[a] == hashes[b]) {
                return false;
            }
        }
    }

    // Test repeated position
    Position pos;
    set_fen(pos, "startpos");
    Position npos = pos;
    make_move(pos, "g1f3");
    make_move(pos, "g8f6");
    make_move(pos, "f3g1");
    make_move(pos, "f6g8");
    if (calculate_hash(pos) != calculate_hash(npos)) {
        return false;
    }

    return true;
}

bool test_uci_pos() {
    const std::pair<std::string, std::string> tests[] = {
        {"startpos",
         "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"},
        {"startpos moves e2e4 c7c5",
         "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"},
        {"fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
         "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"},
    };
    for (const auto &[input, fen] : tests) {
        std::stringstream ss{input};
        Position pos;
        UCI::position(ss, pos);
        if (get_fen(pos) != fen) {
            return false;
        }
    }
    return true;
}

void test() {
    std::cout << (test_fen() ? "Y" : "N") << " -- FEN parsing\n";
    std::cout << (test_perft() ? "Y" : "N") << " -- Perft\n";
    std::cout << (test_flip() ? "Y" : "N") << " -- Flip\n";
    std::cout << (test_hash() ? "Y" : "N") << " -- Hash\n";
    std::cout << (test_uci_pos() ? "Y" : "N") << " -- UCI::position\n";
}
