#pragma once
#include <cstdint>
#include <functional>
#include <random>
#include <string>

class HashFuncGen {
public:
    explicit HashFuncGen(std::uint64_t seed = 123);
    std::function<std::uint32_t(const std::string&)> make_hash();

private:
    std::mt19937_64 rng_;
};
