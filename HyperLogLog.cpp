#include "HyperLogLog.h"
#include <cmath>
#include <algorithm>

HyperLogLog::HyperLogLog(int p, std::function<std::uint32_t(const std::string&)> h)
    : p_(p), h_(std::move(h)), reg_(static_cast<std::size_t>(1ULL << p), 0) {
}

double HyperLogLog::alpha_m(std::size_t m) {
    if (m == 16) return 0.673;
    if (m == 32) return 0.697;
    if (m == 64) return 0.709;
    return 0.7213 / (1.0 + 1.079 / static_cast<double>(m));
}

int HyperLogLog::rho(std::uint32_t w, int max_bits) {
    if (w == 0) return max_bits + 1;
    int shift = 32 - max_bits;
    std::uint32_t x = w << shift;

#if defined(__GNUG__) || defined(__clang__)
    int lz = __builtin_clz(x);
#else
    int lz = 0;
    for (int i = 31; i >= 0; --i) {
        if ((x >> i) & 1U) break;
        ++lz;
    }
#endif
    return lz + 1;
}

void HyperLogLog::add(const std::string& x) {
    std::uint32_t hv = h_(x);
    std::uint32_t idx = hv >> (32 - p_);
    int rem_bits = 32 - p_;
    std::uint32_t mask = (rem_bits == 32) ? 0xFFFFFFFFU : ((1U << rem_bits) - 1U);
    std::uint32_t w = hv & mask;
    int r = rho(w, rem_bits);
    std::uint8_t& cell = reg_[idx];
    cell = static_cast<std::uint8_t>(std::max<int>(cell, r));
}

double HyperLogLog::estimate() const {
    const std::size_t m = reg_.size();
    const double a = alpha_m(m);
    double inv_sum = 0.0;
    std::size_t zeros = 0;
    for (std::uint8_t v : reg_) {
        inv_sum += std::pow(2.0, -static_cast<int>(v));
        if (v == 0) ++zeros;
    }
    double E = a * static_cast<double>(m) * static_cast<double>(m) / inv_sum;
    if (E <= 2.5 * static_cast<double>(m) && zeros > 0) {
        return static_cast<double>(m) * std::log(static_cast<double>(m) / static_cast<double>(zeros));
    }

    return E;
}
