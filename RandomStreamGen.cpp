#include "RandomStreamGen.h"
#include <stdexcept>
#include <algorithm>

RandomStreamGen::RandomStreamGen(Config cfg)
    : cfg_(cfg), rng_(cfg.seed)
{
    if (cfg_.min_len < 0 || cfg_.max_len <= 0 || cfg_.min_len > cfg_.max_len)
        throw std::invalid_argument("Invalid length range");
    if (cfg_.duplicate_prob < 0.0 || cfg_.duplicate_prob > 1.0)
        throw std::invalid_argument("duplicate_prob must be in [0,1]");
}

char RandomStreamGen::random_char() {
    static const std::string alphabet =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "-";
    std::uniform_int_distribution<std::size_t> dist(0, alphabet.size() - 1);
    return alphabet[dist(rng_)];
}

std::string RandomStreamGen::random_string() {
    std::uniform_int_distribution<int> len_dist(cfg_.min_len, cfg_.max_len);
    int len = len_dist(rng_);
    std::string s;
    s.reserve(static_cast<std::size_t>(len));
    for (int i = 0; i < len; ++i) s.push_back(random_char());
    return s;
}

std::vector<std::string> RandomStreamGen::generate_stream() {
    std::vector<std::string> stream;
    stream.reserve(cfg_.stream_size);

    std::vector<std::string> pool;
    pool.reserve(cfg_.stream_size);

    std::uniform_real_distribution<double> prob(0.0, 1.0);

    for (std::size_t i = 0; i < cfg_.stream_size; ++i) {
        std::string x;
        if (!pool.empty() && prob(rng_) < cfg_.duplicate_prob) {
            std::uniform_int_distribution<std::size_t> pick(0, pool.size() - 1);
            x = pool[pick(rng_)];
        }
        else {
            x = random_string();
            pool.push_back(x);
        }
        stream.push_back(std::move(x));
    }
    return stream;
}

std::vector<std::size_t> RandomStreamGen::prefix_sizes_by_percent(std::size_t total, int step_percent) {
    if (step_percent <= 0 || step_percent > 100)
        throw std::invalid_argument("step_percent must be in (0,100]");

    std::vector<std::size_t> out;
    for (int p = step_percent; p <= 100; p += step_percent) {
        std::size_t n = (total * static_cast<std::size_t>(p)) / 100;
        if (n == 0) n = 1;
        out.push_back(n);
    }

    if (out.empty() || out.back() != total) out.push_back(total);

    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}
