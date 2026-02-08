#include "HashFuncGen.h"
#include "MurmurHash3.h"

HashFuncGen::HashFuncGen(std::uint64_t seed) : rng_(seed) {}

std::function<std::uint32_t(const std::string&)> HashFuncGen::make_hash() {
    std::uniform_int_distribution<std::uint32_t> dist;
    std::uint32_t seed = dist(rng_);
    return [seed](const std::string& s) -> std::uint32_t {
        return murmur3_32(s.data(), s.size(), seed);
        };
}
