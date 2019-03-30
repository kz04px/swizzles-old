#ifndef OTHER_HPP_INCLUDED
#define OTHER_HPP_INCLUDED

#include <cassert>
#include <cstdint>

inline int popcountll(const uint64_t n) {
    return __builtin_popcountll(n);
}

inline int lsbll(const uint64_t n) {
    assert(n);
    return __builtin_ctzll(n);
}

inline uint64_t swapll(const uint64_t n) {
    return __builtin_bswap64(n);
}

#endif
