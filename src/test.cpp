#include "test.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "bitboards.hpp"
#include "display.hpp"
#include "fen.hpp"
#include "makemove.hpp"
#include "options.hpp"
#include "perft/perft.hpp"
#include "position.hpp"
#include "protocols.hpp"
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
        hashes.push_back(zobrist::calculate_hash(pos));
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
    if (zobrist::calculate_hash(pos) != zobrist::calculate_hash(npos)) {
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

bool test_uci_moves() {
    const std::pair<std::string, std::string> tests[] = {
        {"e2e4", "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"},
        {"e2e4 c7c5",
         "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"},
        {"e2e4 c7c5 g1f3",
         "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"},
        {"d2d4 g8f6 c2c4 c7c5 d4d5 e7e6 b1c3 e6d5 c4d5 d7d6 e2e4 g7g6",
         "rnbqkb1r/pp3p1p/3p1np1/2pP4/4P3/2N5/PP3PPP/R1BQKBNR w KQkq - 0 7"},
        {"e2e4    ",
         "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"},
        {"e2e4 cat",
         "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"},
    };
    for (const auto &[moves, fen] : tests) {
        std::stringstream ss{moves};
        Position pos;
        set_fen(pos, "startpos");
        UCI::moves(ss, pos);
        if (get_fen(pos) != fen) {
            std::cout << "Got: " << get_fen(pos) << std::endl;
            return false;
        }
    }
    return true;
}

bool test_options() {
    options::spins["val"] = options::Spin(1, 2, 3);
    options::combos["A B"] = options::Combo("A", {"A", "B"});

    if (options::spins["val"].min_ != 1) {
        return false;
    }
    if (options::spins["val"].val_ != 2) {
        return false;
    }
    if (options::spins["val"].max_ != 3) {
        return false;
    }

    options::set("val", "1");
    if (options::spins["val"].val_ != 1) {
        return false;
    }
    options::set("val", "0");
    if (options::spins["val"].val_ != 1) {
        return false;
    }
    options::set("val", "4");
    if (options::spins["val"].val_ != 1) {
        return false;
    }

    if (options::combos["A B"].val_ != "A") {
        return false;
    }
    options::set("A B", "B");
    if (options::combos["A B"].val_ != "B") {
        return false;
    }
    options::set("A B", "C");
    if (options::combos["A B"].val_ != "B") {
        return false;
    }

    return true;
}

bool test_threefold() {
    std::vector<std::tuple<std::string, int, std::vector<std::string>>> tests =
        {
            {"startpos", 0, {""}},
            {"startpos", 0, {"e2e4", "c7c5"}},
            {"startpos", 1, {"g1f3", "g8f6", "f3g1", "f6g8"}},
            {"startpos",
             2,
             {"g1f3", "g8f6", "f3g1", "f6g8", "g1f3", "g8f6", "f3g1", "f6g8"}},
        };

    for (const auto &[fen, threefold, moves] : tests) {
        Position pos;
        set_fen(pos, fen);
        for (const auto &move : moves) {
            make_move(pos, move);
        }
        if (repetitions(pos) != threefold) {
            return false;
        }
    }

    return true;
}

bool test_files() {
    std::vector<std::pair<std::string, uint64_t>> tests = {
        {"startpos", 0ULL},
        {"4k3/4p1p1/8/8/8/8/PP6/4K3 w - -",
         U64_FILE_C | U64_FILE_D | U64_FILE_F | U64_FILE_H},
    };

    for (const auto &[fen, answer] : tests) {
        Position pos;
        set_fen(pos, fen);
        if (answer != open_files(pos.pieces[PieceType::PAWN])) {
            return false;
        }
    }

    return true;
}

bool test_passers() {
    std::vector<std::pair<std::string, uint64_t>> tests = {
        {"startpos", 0ULL},
        {"4k3/2P5/8/Pp6/1p3p1P/2P2P1p/3P4/4K3 w - -", 0x4000180000800},
        {"4k3/2p2p2/1pP4p/3p4/8/8/p3P3/4K3 b - -", 0x1000000820000},
    };

    for (const auto &[fen, answer] : tests) {
        Position pos;
        set_fen(pos, fen);
        const uint64_t us =
            pos.colour[Colour::US] & pos.pieces[PieceType::PAWN];
        const uint64_t them =
            pos.colour[Colour::THEM] & pos.pieces[PieceType::PAWN];
        if (answer != passed_pawns(us, them)) {
            return false;
        }
    }

    return true;
}

bool test_chains() {
    std::vector<std::pair<std::string, uint64_t>> tests = {
        {"startpos", 0ULL},
        {"4k3/2P5/8/Pp6/1p3p1P/2P2P1p/3P4/4K3 w - -", 0x40000},
        {"4k3/2p2p2/1pP4p/3p4/8/8/p3P3/4K3 b - -", 0x20000},
        {"4k3/8/8/P6P/P6P/P6P/P6P/4K3 w - -", 0x0},
        {"4k3/8/8/3PP3/2P2P2/1P4P1/P6P/4K3 w - -", 0x1824420000},
    };

    for (const auto &[fen, answer] : tests) {
        Position pos;
        set_fen(pos, fen);
        const uint64_t pawns =
            pos.colour[Colour::US] & pos.pieces[PieceType::PAWN];
        if (answer != chained_pawns(pawns)) {
            return false;
        }
    }

    return true;
}

bool test_backwards() {
    std::vector<std::pair<std::string, uint64_t>> tests = {
        {"startpos", 0ULL},
        {"4k3/8/8/8/p2p2p1/8/1PP2P2/4K3 w - -", 0x2000},
        {"4k3/1p6/4p3/3P4/6Pp/8/P1P2P2/4K3 w - -", 0ULL},
    };

    for (const auto &[fen, answer] : tests) {
        Position pos;
        set_fen(pos, fen);
        const uint64_t us =
            pos.colour[Colour::US] & pos.pieces[PieceType::PAWN];
        const uint64_t them =
            pos.colour[Colour::THEM] & pos.pieces[PieceType::PAWN];
        if (answer != backward_pawns(us, them)) {
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
    std::cout << (test_threefold() ? "Y" : "N") << " -- Threefold\n";
    std::cout << (test_files() ? "Y" : "N") << " -- Files\n";
    std::cout << (test_passers() ? "Y" : "N") << " -- Pawns - passed\n";
    std::cout << (test_chains() ? "Y" : "N") << " -- Pawns - chained\n";
    std::cout << (test_backwards() ? "Y" : "N") << " -- Pawns - backwards\n";
    std::cout << (test_options() ? "Y" : "N") << " -- Options\n";
    std::cout << (test_uci_pos() ? "Y" : "N") << " -- UCI::position\n";
    std::cout << (test_uci_moves() ? "Y" : "N") << " -- UCI::moves\n";
}
