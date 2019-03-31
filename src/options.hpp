#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace options {

struct Spin {
    Spin() : min_(0), val_(0), max_(0) {
    }
    Spin(const int a, const int b, const int c) : min_(a), val_(b), max_(c) {
    }
    int min_;
    int val_;
    int max_;
};

struct Combo {
    Combo() : val_(), choices_() {
    }
    Combo(const std::string &a, const std::vector<std::string> &b)
        : val_(a), choices_(b) {
    }
    std::string val_;
    std::vector<std::string> choices_;
};

extern std::unordered_map<std::string, Spin> spins;
extern std::unordered_map<std::string, Combo> combos;

void set(const std::string &name, const std::string &value);

namespace uci {

void print();

}  // namespace uci

}  // namespace options

#endif
