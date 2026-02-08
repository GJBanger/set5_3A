#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <functional>

class HyperLogLog {
public:
    explicit HyperLogLog(int p, std::function<std::uint32_t(const std::string&)> h);

    void add(const std::string& x);
    double estimate() const;

private:
    int p_;
    std::function<std::uint32_t(const std::string&)> h_;
    std::vector<std::uint8_t> reg_;

    static double alpha_m(std::size_t m);
    static int rho(std::uint32_t w, int max_bits);
};
