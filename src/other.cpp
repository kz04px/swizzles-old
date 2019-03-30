#include "other.hpp"
#include <iostream>

int popcountll(const uint64_t n) {
    return __builtin_popcountll(n);
}

int lsbll(const uint64_t n) {
    return __builtin_ctzll(n);
}

uint64_t swapll(const uint64_t n) {
    return __builtin_bswap64(n);
}

void print_u64(const uint64_t n) {
    for (int y = 7; y >= 0; --y) {
        for (int x = 0; x < 8; ++x) {
            int sq = y * 8 + x;
            uint64_t bb = (1ULL) << sq;

            if (n & bb) {
                std::cout << "1";
            } else {
                std::cout << "0";
            }
        }
        std::cout << std::endl;
    }
}
