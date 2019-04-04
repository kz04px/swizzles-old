#include <iostream>
#include "bitboards.hpp"
#include "hashtable.hpp"
#include "players/mcts-uct/node.hpp"
#include "players/mcts-uct/state.hpp"
#include "protocols.hpp"
#include "test.hpp"
#include "zobrist.hpp"

int main() {
    srand(time(0));
    bitboards_init();
    zobrist_init();

    std::string mode;
    std::cin >> mode;

    if (mode == "uci") {
        UCI::listen();
    } else if (mode == "xboard") {
        std::cout << "xboard protocol not supported" << std::endl;
    } else if (mode == "test") {
        test();
    } else if (mode == "about") {
        std::cout << "Chess engine written in C++" << std::endl;
        std::cout << "By kz04px" << std::endl;
        std::cout << "Date: " << __DATE__ << std::endl;
        std::cout << "Time: " << __TIME__ << std::endl;
        std::cout << std::endl;

        std::cout << "Debug:" << std::endl;
        std::cout << "sizeof(Move) = " << sizeof(Move) << "B" << std::endl;
        std::cout << "sizeof(Position) = " << sizeof(Position) << "B"
                  << std::endl;
        std::cout << "sizeof(Entry) = " << sizeof(Entry) << "B" << std::endl;
        std::cout << "sizeof(Node) = " << sizeof(Node) << "B" << std::endl;
        std::cout << "sizeof(State) = " << sizeof(State) << "B" << std::endl;
#ifdef NDEBUG
        std::cout << "Release build" << std::endl;
#else
        std::cout << "Debug build" << std::endl;
#endif
    } else {
        std::cout << "Unknown command" << std::endl;
    }

    return 0;
}
