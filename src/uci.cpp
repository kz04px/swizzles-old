#include "uci.hpp"
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include "display.hpp"
#include "fen.hpp"
#include "hashtable.hpp"
#include "makemove.hpp"
#include "perft.hpp"
#include "position.hpp"
#include "search.hpp"

Position pos;
Hashtable tt;
std::thread search_thread;
bool stop_search = false;

namespace UCI {

namespace Extension {

void print() {
    display(pos);
}

void perft(std::stringstream& ss) {
    int depth = 0;
    ss >> depth;

    uint64_t nodes = 0ULL;
    for (int i = 1; i <= depth; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        nodes = perft(pos, i);
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;

        std::cout << "info"
                  << " depth " << i << " nodes " << nodes << " time "
                  << static_cast<int>(elapsed.count() * 1000) << " nps "
                  << static_cast<int>(nodes / elapsed.count()) << std::endl;
    }

    std::cout << "nodes " << nodes << std::endl;
}

void ttperft(std::stringstream& ss) {
}

}  // namespace Extension

void stop() {
    if (search_thread.joinable()) {
        stop_search = true;
        search_thread.join();
    }
    stop_search = false;
}

void ucinewgame() {
    stop();
    set_fen(pos, "startpos");
    tt.clear();
}

void go(std::stringstream& ss) {
    stop();

    SearchOptions options;

    // Subcommands
    std::string word;
    while (ss >> word) {
        if (word == "infinite") {
            options.type = SearchType::Depth;
            options.depth = MAX_DEPTH;
        } else if (word == "depth") {
            options.type = SearchType::Depth;
            ss >> options.depth;
        } else if (word == "nodes") {
            options.type = SearchType::Nodes;
            ss >> options.nodes;
        } else if (word == "movetime") {
            options.type = SearchType::Movetime;
            ss >> options.movetime;
        } else if (word == "movestogo") {
            ss >> options.movestogo;
        } else if (word == "wtime") {
            options.type = SearchType::Time;
            ss >> options.wtime;
        } else if (word == "btime") {
            options.type = SearchType::Time;
            ss >> options.btime;
        } else if (word == "winc") {
            options.type = SearchType::Time;
            ss >> options.winc;
        } else if (word == "binc") {
            options.type = SearchType::Time;
            ss >> options.binc;
        }
    }

    // Alpha-beta
    search_thread = std::thread(search,
                                std::ref(pos),
                                std::ref(tt),
                                std::ref(stop_search),
                                std::ref(options));
}

void isready() {
    std::cout << "readyok" << std::endl;
}

void moves(std::stringstream& ss) {
    std::string word;
    while (ss >> word) {
        if (legal_move(pos, word) == false) {
            break;
        }
        make_move(pos, word);
    }
}

void position(std::stringstream& ss) {
    std::string fen = "";
    std::string word;

    // Gather fen string
    while (ss >> word && word != "moves") {
        if (fen == "") {
            fen = word;
        } else {
            fen += word;
        }
    }

    // Replacements
    if (fen == "startpos") {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    }

    // Set fen
    if (fen != "") {
        bool r = set_fen(pos, fen);
        if (r == false) {
            std::cout << "WARNING: set position error (" << r << ")"
                      << std::endl;
        }
    }

    // Call moves() if found
    if (word == "moves") {
        moves(ss);
    }
}

void setoption(std::stringstream& ss) {
}

void listen() {
    std::cout << "id name Swizzles" << std::endl;
    std::cout << "id author kz04px" << std::endl;
    std::cout << "uciok" << std::endl;

    std::string word;
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        std::stringstream ss{line};
        ss >> word;

        if (word == "isready") {
            isready();
            break;
        } else if (word == "setoption") {
            setoption(ss);
        } else if (word == "quit") {
            return;
        }
    }

    set_fen(pos, "startpos");

    tt.create(128);
    ucinewgame();

    bool quit = false;
    while (!quit) {
        std::getline(std::cin, line);
        std::stringstream ss{line};
        ss >> word;

        if (word == "go") {
            go(ss);
        } else if (word == "isready") {
            isready();
        } else if (word == "moves") {
            moves(ss);
        } else if (word == "perft") {
            Extension::perft(ss);
        } else if (word == "position") {
            position(ss);
        } else if (word == "print") {
            Extension::print();
        } else if (word == "quit") {
            quit = true;
        } else if (word == "stop") {
            stop();
        } else if (word == "ttperft") {
            Extension::ttperft(ss);
        } else if (word == "ucinewgame") {
            ucinewgame();
        }
    }

    stop();
}

}  // namespace UCI
