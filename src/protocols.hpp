#ifndef UCI_HPP_INCLUDED
#define UCI_HPP_INCLUDED

#include <sstream>
#include "hashtable.hpp"
#include "position.hpp"

namespace UCI {

void ucinewgame(const Position& pos, Hashtable& tt);
void go(std::stringstream& ss, const Position& pos);
void moves(std::stringstream& ss, Position& pos);
void position(std::stringstream& ss, Position& pos);
void setoption(std::stringstream& ss);
void listen();

}  // namespace UCI

#endif
