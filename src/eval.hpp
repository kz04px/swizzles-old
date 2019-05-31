#ifndef EVAL_HPP
#define EVAL_HPP

struct Position;

const int piece_value[7] = {100, 300, 325, 500, 900, 1000000, 0};

int eval(const Position &pos);

#endif
