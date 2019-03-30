#ifndef OTHER_HPP_INCLUDED
#define OTHER_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string &s, char delim);
int popcountll(const uint64_t n);
int lsbll(const uint64_t n);
uint64_t swapll(const uint64_t n);
void print_u64(const uint64_t n);

#endif
