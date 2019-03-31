#include "options.hpp"
#include <iostream>

namespace options {

std::unordered_map<std::string, Spin> spins;
std::unordered_map<std::string, Combo> combos;

void set(const std::string &name, const std::string &value) {
    // Spins
    if (options::spins.find(name) != options::spins.end()) {
        int n = std::stoi(value);
        if (options::spins[name].min_ <= n && n <= options::spins[name].max_) {
            options::spins[name].val_ = n;
        }
    }
    // Combos
    else if (options::combos.find(name) != options::combos.end()) {
        for (const auto &str : options::combos[name].choices_) {
            if (str == value) {
                options::combos[name].val_ = value;
                break;
            }
        }
    }
}

namespace uci {

void print() {
    // Spins
    for (const auto &[str, spin] : spins) {
        std::cout << "option name " << str << " type spin default " << spin.val_
                  << " min " << spin.min_ << " max " << spin.max_ << std::endl;
    }
    // Combos
    for (const auto &[str, combo] : combos) {
        std::cout << "option name " << str << " type combo default "
                  << combo.val_;
        for (const auto &v : combo.choices_) {
            std::cout << " var " << v;
        }
        std::cout << std::endl;
    }
}

}  // namespace uci

}  // namespace options
