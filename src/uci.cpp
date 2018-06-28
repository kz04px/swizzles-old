#include <iostream>
#include <ctime>
#include <thread>
#include <string>
#include <vector>
#include "uci.hpp"
#include "position.hpp"
#include "fen.hpp"
#include "attacks.hpp"
#include "movegen.hpp"
#include "makemove.hpp"
#include "move.hpp"
#include "perft.hpp"
#include "display.hpp"
#include "legal.hpp"
#include "search.hpp"
#include "hashtable.hpp"
#include "other.hpp"

void uci()
{
    std::cout << "id name Swizzles" << std::endl;
    std::cout << "id author kz04px" << std::endl;
    std::cout << "uciok" << std::endl;

    Position pos;
    set_fen(pos, "startpos");

    Hashtable tt = Hashtable(128);
    tt.clear();

    bool stop = false;
    std::thread search_thread;

    bool quit = false;
    while(quit == false)
    {
        std::string input;
        getline(std::cin, input);

        std::vector<std::string> tokens = split(input, ' ');

        for(unsigned int n = 0; n < tokens.size(); ++n)
        {
            if(tokens[n] == "isready")
            {
                std::cout << "readyok" << std::endl;
            }
            else if(tokens[n] == "ucinewgame")
            {
                set_fen(pos, "startpos");
                tt.clear();
            }
            else if(tokens[n] == "go")
            {
                // Stop the search if there's already one going
                if(search_thread.joinable())
                {
                    stop = true;
                    search_thread.join();
                    stop = false;
                }

                int depth = -1;
                int movetime = -1;
                int nodes = -1;
                bool infinite = false;
                int wtime = -1;
                int btime = -1;
                int winc = -1;
                int binc = -1;
                int movestogo = -1;

                // Subcommands
                for(unsigned int i = n+1; i < tokens.size(); ++i)
                {
                    if(tokens[i] == "infinite")
                    {
                        infinite = true;
                        n += 1;
                    }
                    else if(tokens[i] == "depth")
                    {
                        depth = std::stoi(tokens[i+1]);
                        n += 2;
                        i += 1;
                    }
                    else if(tokens[i] == "nodes")
                    {
                        nodes = std::stoi(tokens[i+1]);
                        n += 2;
                        i += 1;
                    }
                    else if(tokens[i] == "movetime")
                    {
                        movetime = std::stoi(tokens[i+1]);
                        n += 2;
                        i += 1;
                    }
                    else if(tokens[i] == "movestogo")
                    {
                        movestogo = std::stoi(tokens[i+1]);
                        n += 2;
                        i += 1;
                    }
                    else if(tokens[i] == "wtime")
                    {
                        wtime = std::stoi(tokens[i+1]);
                        n += 2;
                        i += 1;
                    }
                    else if(tokens[i] == "btime")
                    {
                        btime = std::stoi(tokens[i+1]);
                        n += 2;
                        i += 1;
                    }
                    else if(tokens[i] == "winc")
                    {
                        winc = std::stoi(tokens[i+1]);
                        n += 2;
                        i += 1;
                    }
                    else if(tokens[i] == "binc")
                    {
                        binc = std::stoi(tokens[i+1]);
                        n += 2;
                        i += 1;
                    }
                }

                // Alpha-beta
                search_thread = std::thread(search, std::ref(pos), std::ref(tt), std::ref(stop), depth, movetime, nodes, infinite, wtime, btime, winc, binc, movestogo);
            }
            else if(tokens[n] == "stop")
            {
                if(search_thread.joinable())
                {
                    stop = true;
                    search_thread.join();
                    stop = false;
                }
            }
            else if(tokens[n] == "perft")
            {
                int depth = 5;

                // Subcommands
                if(n+1 < tokens.size())
                {
                    depth = std::stoi(tokens[n+1]);
                    n += 1;
                }

                // Subcommand checking
                if(depth < 1)
                {
                    depth = 1;
                }

                uint64_t nodes = 0ULL;
                for(int i = 1; i <= depth; ++i)
                {
                    clock_t start = clock();
                    nodes = perft(pos, i);
                    clock_t end = clock();
                    double time_taken = (double)(end - start)/CLOCKS_PER_SEC;

                    std::cout << "info"
                              << " depth " << i
                              << " nodes " << nodes
                              << " nps " << (int)((double)nodes/time_taken)
                              << " time " << (int)(1000.0*time_taken)
                              << std::endl;
                }
                std::cout << "nodes " << nodes << std::endl;
            }
            else if(tokens[n] == "print" || tokens[n] == "display")
            {
                display(pos);
            }
            else if(tokens[n] == "position")
            {
                if(n+1 < tokens.size())
                {
                    n += 1;
                    if(tokens[n] == "startpos")
                    {
                        bool r = set_fen(pos, "startpos");
                        if(r == false)
                        {
                            std::cout << "WARNING: set position error (" << r << ")" << std::endl;
                        }
                    }
                    else if(tokens[n] == "fen")
                    {
                        if(n+1 < tokens.size())
                        {
                            n += 1;
                            std::string fen_string = tokens[n];

                            while(n+1 < tokens.size() && tokens[n+1] != "moves")
                            {
                                n += 1;
                                fen_string += " " + tokens[n];
                            }

                            bool r = set_fen(pos, fen_string);
                            if(r == false)
                            {
                                std::cout << "WARNING: set position error (" << r << ")" << std::endl;
                            }
                        }
                    }
                }
            }
            else if(tokens[n] == "moves")
            {
                for(unsigned int i = n+1; i < tokens.size(); ++i)
                {
                    if(legal_move(pos, tokens[i]) == false) {break;}

                    make_move(pos, tokens[i]);
                    n += 1;
                }
            }
            else if(tokens[n] == "quit")
            {
                quit = true;
                break;
            }
        }
    }

    if(search_thread.joinable())
    {
        stop = true;
        search_thread.join();
        stop = false;
    }
}
