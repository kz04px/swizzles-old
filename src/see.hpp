#ifndef SEE_HPP_INCLUDED
#define SEE_HPP_INCLUDED

struct Position;
struct Move;

int see_capture(const Position &pos, const Move &move);
int see_quiet(const Position &pos, const Move &move);

#endif
