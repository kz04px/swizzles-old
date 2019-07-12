#include "tune.hpp"
#include <chrono>
#include <cmath>
#include <fstream>
#include <future>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include "../fen.hpp"
#include "qsearch.hpp"

#define MAX_ENTRIES 100000

struct Data {
    Position pos_;
    float score_;
};

float sigmoid(const float s, const float k = 1.13) {
    return 1.0 / (1.0 + pow(10, s * -k / 400.0));
}

float E(const std::vector<int> &values,
        std::vector<Data>::const_iterator start,
        std::vector<Data>::const_iterator end) {
    assert(start < end);
    float sum = 0.0;

    while (start != end) {
        auto qscore = tune_qsearch(start->pos_, -INF, INF, values);
        if (start->pos_.flipped) {
            qscore = -qscore;
        }
        const auto n = start->score_ - sigmoid(qscore);
        sum += n * n;

        start++;
    }

    assert(sum >= 0.0);
    return sum;
}

template <class T>
float get(const int num_threads,
          const std::vector<Data> &entries,
          const std::vector<T> &values) {
    // Futures
    std::vector<std::future<float>> futures;

    // Start threads
    std::vector<Data>::const_iterator iter = entries.begin();
    const auto spacer = entries.size() / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        std::vector<Data>::const_iterator start = iter;
        std::vector<Data>::const_iterator end = iter + spacer;
        iter = end + 1;
        if (iter > entries.end()) {
            iter = entries.end();
        }
        futures.push_back(
            std::async(std::launch::async, E, std::cref(values), start, end));
    }

    // Wait
    float sum = 0.0;
    for (auto &f : futures) {
        f.wait();
        sum += f.get();
    }

    return sum;
}

void tune(const std::string &path, const int num_threads) {
    std::vector<Data> entries = {};
    int num_entries = 0;
    int num_iterations = 0;

    // Load .epd
    {
        std::fstream file(path);

        if (!file.is_open()) {
            std::cerr << "Could not open file " << path << std::endl;
            return;
        }

        std::string line;
        while (num_entries < MAX_ENTRIES && getline(file, line) &&
               !line.empty()) {
            std::stringstream ss{line};
            std::string fen;
            float score = -2.0;

            // Parse line
            std::string word;
            while (ss >> word) {
                if (word == "c9" && ss >> word && !word.empty()) {
                    if (word == "\"1/2-1/2\";") {
                        score = 0.5;
                    } else if (word == "\"1-0\";") {
                        score = 1.0;
                    } else if (word == "\"0-1\";") {
                        score = 0.0;
                    } else {
                        std::cerr << "Error: " << line << std::endl;
                    }
                    break;
                }

                if (fen.empty()) {
                    fen = word;
                } else {
                    fen += " " + word;
                }
            }

            // Create position
            Position pos;
            const auto n = set_fen(pos, fen);

            assert(n);
            assert(score > -1.5);

            // Store
            entries.push_back(Data{.pos_ = pos, .score_ = score});

            num_entries++;
        }
    }

    std::cout << "Positions loaded: " << num_entries << std::endl;

    // Create values to tune
    std::vector<int> values = {142, 476, 493, 789, 1542};

    const auto original_e = get(num_threads, entries, values);
    auto best_e = original_e;

    const auto start = std::chrono::high_resolution_clock::now();

    std::stack<int> increments({1, 2, 5, 10, 20});

    int inc = increments.top();
    increments.pop();

    // Run
    bool improved = true;
    while (improved) {
        num_iterations++;
        improved = false;

        // Status update
        if (num_iterations == 1 || num_iterations % 1 == 0) {
            const auto now = std::chrono::high_resolution_clock::now();
            const auto ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                                      start);
            const int s = ms.count() / 1000.0;

            std::cout << "Iteration: " << num_iterations << std::endl;
            std::cout << "Error: " << best_e << std::endl;
            std::cout << "Increment: " << inc << std::endl;
            std::cout << "Values:";
            for (const auto &val : values) {
                std::cout << " " << val;
            }
            std::cout << std::endl;
            std::cout << "Time: " << s << "s" << std::endl;
            if (s > 0) {
                std::cout << "Pos/s: " << num_entries * num_iterations / s
                          << std::endl;
            }
            std::cout << std::endl;
        }

        // Try improve
        for (auto &val : values) {
            val += inc;
            auto new_e = get(num_threads, entries, values);
            if (new_e < best_e) {
                best_e = new_e;
                improved = true;
            } else {
                val -= 2 * inc;
                new_e = get(num_threads, entries, values);
                if (new_e < best_e) {
                    best_e = new_e;
                    improved = true;
                } else {
                    val += inc;
                }
            }
        }

        // Lower increment if we can
        if (!improved && !increments.empty()) {
            std::cout << "Reducing increment size from " << inc << " to "
                      << increments.top() << std::endl;
            std::cout << std::endl;

            inc = increments.top();
            increments.pop();
            improved = true;
        }
    }
}
