#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <iostream>
#include <sstream>
#include "fen.hpp"
#include "perft/perft.hpp"
#include "players/all.hpp"
#include "position.hpp"

using namespace std::chrono;

void benchmark() {
    int total = 0;

    {
        // Perft
        Position pos;
        set_fen(pos, "startpos");
        const auto t0 = high_resolution_clock::now();
        perft(pos, 5);
        const auto t1 = high_resolution_clock::now();
        const auto diff = duration_cast<milliseconds>(t1 - t0);
        std::cout << "Perft:  " << diff.count() << "ms" << std::endl;
        total += diff.count();
    }

    {
        // Search
        Hashtable tt(16);
        SearchOptions options;
        options.type = SearchType::Depth;
        options.depth = 9;
        bool stop = false;
        Position pos;
        set_fen(pos, "startpos");
        std::stringstream buffer;
        std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
        const auto t0 = high_resolution_clock::now();
        player::alphabeta(pos, tt, stop, options);
        const auto t1 = high_resolution_clock::now();
        std::cout.rdbuf(old);
        const auto diff = duration_cast<milliseconds>(t1 - t0);
        std::cout << "Search: " << diff.count() << "ms" << std::endl;
        total += diff.count();
    }

    std::cout << "Total: " << total << "ms" << std::endl;
}

#endif
