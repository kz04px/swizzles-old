#include <iostream>
#include "test.hpp"
#include "position.hpp"
#include "zobrist.hpp"
#include "fen.hpp"
#include "flip.hpp"
#include "makemove.hpp"
#include "perft.hpp"

struct Board
{
    std::string fen;
    uint64_t nodes;
};

const Board boards[] = {
    Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 197281},
    Board{"r2q1rk1/pp1bppbp/2np1np1/8/2BNP3/2N1BP2/PPPQ2PP/R3K2R w KQ - 5 10", 3626712},
    Board{"rnbqk2r/pp3pbp/3p1np1/2pP4/4PP2/2N5/PP4PP/R1BQKBNR w KQkq - 1 8", 1635313}
};

bool test()
{
    for(auto &n : boards)
    {
        Position pos;

        // Test set_fen() -- 1
        if(set_fen(pos, n.fen) == false)
        {
            std::cout << "set_fen() fail on " << n.fen << std::endl;
            return false;
        }

        // Test set_fen() -- 2
        if(get_fen(pos) != n.fen)
        {
            std::cout << "set_fen() get_fen() mismatch on " << n.fen << std::endl;
            return false;
        }

        // Test flip()
        uint64_t key1 = calculate_hash(pos);
        flip(pos);
        flip(pos);
        uint64_t key2 = calculate_hash(pos);
        if(key1 != key2)
        {
            std::cout << "double flip() hash mismatch on " << n.fen << std::endl;
            return false;
        }

        // Test Position operator ==
        Position npos = pos;
        flip(npos);
        flip(npos);
        if(pos != npos)
        {
            std::cout << "position equality operator mismatch on " << n.fen << std::endl;
            return false;
        }

        // Test perft()
        if(perft(pos, 4) != n.nodes)
        {
            std::cout << "perft() fail on " << n.fen << std::endl;
            return false;
        }
    }

    std::cout << "All tests passed" << std::endl;
    return true;
}
