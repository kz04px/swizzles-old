#include <chrono>
#include <iostream>
#include <thread>
#include "display.hpp"
#include "fen.hpp"
#include "makemove.hpp"
#include "mcts-uct.hpp"
#include "options.hpp"
#include "protocols.hpp"
#include "search.hpp"

std::thread search_thread;
bool stop_search = false;

namespace UCI {

namespace Extension {

void options() {
    options::uci::print();
}

void print(const Position& pos) {
    display(pos);
}

void perft(std::stringstream& ss, const Position& pos) {
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

void ttperft(std::stringstream& ss, const Position& pos, Hashtable& tt) {
}

}  // namespace Extension

void stop() {
    if (search_thread.joinable()) {
        stop_search = true;
        search_thread.join();
    }
    stop_search = false;
}

void ucinewgame(Position& pos, Hashtable& tt) {
    stop();
    set_fen(pos, "startpos");
    tt.clear();
}

void go(std::stringstream& ss, const Position& pos, Hashtable& tt) {
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

    if (options::combos["Search"].val_ == "mcts") {
        search_thread = std::thread(mcts_uct,
                                    std::ref(pos),
                                    std::ref(tt),
                                    std::ref(stop_search),
                                    options.movetime,
                                    options.nodes,
                                    false,
                                    options.wtime,
                                    options.btime,
                                    options.winc,
                                    options.binc,
                                    options.movestogo);
    } else {
        // Default search is alphabeta
        search_thread = std::thread(search,
                                    std::ref(pos),
                                    std::ref(tt),
                                    std::ref(stop_search),
                                    options);
    }
}

void isready() {
    std::cout << "readyok" << std::endl;
}

void moves(std::stringstream& ss, Position& pos) {
    std::string word;
    while (ss >> word) {
        if (legal_move(pos, word) == false) {
            break;
        }
        make_move(pos, word);
    }
}

void position(std::stringstream& ss, Position& pos) {
    std::string fen = "";
    std::string word;

    ss >> word;

    if (word == "startpos") {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        ss >> word;
    } else if (word == "fen") {
        // Gather fen string
        while (ss >> word && word != "moves") {
            if (fen == "") {
                fen = word;
            } else {
                fen += " " + word;
            }
        }
    }

    if (fen == "") {
        return;
    }

    // Set fen
    bool r = set_fen(pos, fen);
    if (!r) {
        std::cout << "WARNING: set position error" << std::endl;
    }

    // Call moves() if found
    if (word == "moves") {
        moves(ss, pos);
    }
}

void setoption(std::stringstream& ss) {
    std::string word;
    std::string name = "";
    std::string value = "";

    ss >> word;
    if (word != "name") {
        return;
    }

    // Gather option name
    while (ss >> word && word != "value") {
        if (name == "") {
            name = word;
        } else {
            name += " " + word;
        }
    }

    // Gather option value
    while (ss >> word) {
        if (value == "") {
            value = word;
        } else {
            value += " " + word;
        }
    }

    options::set(name, value);
}

void listen() {
    // Create options
    options::spins["Hash"] = options::Spin(1, 128, 2048);
    options::combos["Search"] =
        options::Combo("alphabeta", {"alphabeta", "mcts"});

    std::cout << "id name Swizzles" << std::endl;
    std::cout << "id author kz04px" << std::endl;

    options::uci::print();

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

    assert(options::spins["Hash"].val_ > 0);

    Hashtable tt(options::spins["Hash"].val_);
    Position pos;
    set_fen(pos, "startpos");
    ucinewgame(pos, tt);

    bool quit = false;
    while (!quit) {
        std::getline(std::cin, line);
        std::stringstream ss{line};
        ss >> word;

        if (word == "go") {
            go(ss, pos, tt);
        } else if (word == "isready") {
            isready();
        } else if (word == "moves") {
            moves(ss, pos);
        } else if (word == "options") {
            Extension::options();
        } else if (word == "perft") {
            Extension::perft(ss, pos);
        } else if (word == "position") {
            position(ss, pos);
        } else if (word == "print") {
            Extension::print(pos);
        } else if (word == "quit") {
            quit = true;
        } else if (word == "stop") {
            stop();
        } else if (word == "ttperft") {
            Extension::ttperft(ss, pos, tt);
        } else if (word == "ucinewgame") {
            ucinewgame(pos, tt);
        }
    }

    stop();
}

}  // namespace UCI
