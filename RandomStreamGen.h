#pragma once
#include <string>
#include <vector>
#include <random>
#include <cstddef>

class RandomStreamGen {
public:
    struct Config {
        std::size_t stream_size = 200000;
        int min_len = 1;
        int max_len = 30;
        double duplicate_prob = 0.10;
        std::uint64_t seed = 42;
    };
    explicit RandomStreamGen(Config cfg);
    std::vector<std::string> generate_stream();
    static std::vector<std::size_t> prefix_sizes_by_percent(std::size_t total, int step_percent);
private:
    Config cfg_;
    std::mt19937_64 rng_;

    char random_char();
    std::string random_string();
};
