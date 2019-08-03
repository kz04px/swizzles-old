#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "fen.hpp"
#include "perft/perft.hpp"
#include "players/all.hpp"
#include "position.hpp"

using namespace std::chrono;

void benchmark() {
    int total = 0;

    {
        // Perft
        const std::vector<std::string> fens = {
            "startpos",
            "2rq1rk1/pp1bppbp/3p1np1/4n3/3NP3/1BN1BP2/PPPQ2PP/2KR3R w - - 9 12",
        };

        Position pos;
        const auto t0 = high_resolution_clock::now();
        for (const auto &fen : fens) {
            set_fen(pos, fen);
            perft(pos, 5);
        }

        const auto t1 = high_resolution_clock::now();
        const auto diff = duration_cast<milliseconds>(t1 - t0);
        std::cout << "Perft:  " << diff.count() << "ms" << std::endl;
        total += diff.count();
    }

    {
        // Search
        const std::vector<std::string> fens = {
            "startpos",
            "2rq1rk1/pp1bppbp/3p1np1/4n3/3NP3/1BN1BP2/PPPQ2PP/2KR3R w - - 9 12",
            "r1bqr1k1/1p1n1pbp/p2p2p1/2pP4/Pn2PP2/2N2N2/1P4PP/R1BQRBK1 w - - 2 "
            "14",
            "6k1/1b6/2p1p2p/1pPpPpbB/1P3B2/6P1/1q2P2P/5QK1 w - f6 0 30",
            "2r3k1/5p1p/p3r1p1/4p1QP/2P1P3/8/q4PP1/2R1R1K1 b - - 0 24",
        };

        Hashtable tt(16);
        SearchOptions options;
        options.type = SearchType::Depth;
        options.depth = 9;
        bool stop = false;
        Position pos;
        std::stringstream buffer;
        std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
        const auto t0 = high_resolution_clock::now();
        for (const auto &fen : fens) {
            set_fen(pos, fen);
            player::alphabeta(pos, tt, stop, options);
        }
        const auto t1 = high_resolution_clock::now();
        const auto diff = duration_cast<milliseconds>(t1 - t0);
        std::cout.rdbuf(old);
        std::cout << "Search: " << diff.count() << "ms" << std::endl;
        total += diff.count();
    }

    std::cout << "Total: " << total << "ms" << std::endl;
}

#endif
