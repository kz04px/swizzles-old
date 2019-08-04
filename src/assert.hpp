#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <iostream>

#ifdef NDEBUG
#define UCI_ASSERT(condition)
#else
#define UCI_ASSERT(condition)                                            \
    {                                                                    \
        if (!(condition)) {                                              \
            std::cout << "info string assert failed" << std::endl;       \
            std::cout << "info string file: " << __FILE__ << std::endl;  \
            std::cout << "info string line: " << __LINE__ << std::endl;  \
            std::cout << "info string function: " << __PRETTY_FUNCTION__ \
                      << std::endl;                                      \
            std::cout << "info string condition: " << (#condition)       \
                      << std::endl;                                      \
            abort();                                                     \
        }                                                                \
    }
#endif

#endif
