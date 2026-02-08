#include "RandomStreamGen.h"
#include "HashFuncGen.h"
#include "HyperLogLog.h"
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

struct Point {
    std::size_t t;
    std::size_t prefix;
    double trueF0;
    double estN;
};

int main() {
    const int B = 12;
    const int step_percent = 5;
    const std::size_t stream_size = 200000;
    const int num_streams = 30;
    const double dup_prob = 0.15;
    HashFuncGen hgen(777);
    auto h = hgen.make_hash();
    std::vector<std::size_t> pref = RandomStreamGen::prefix_sizes_by_percent(stream_size, step_percent);
    std::size_t T = pref.size();
    std::vector<std::vector<double>> allN(num_streams, std::vector<double>(T, 0.0));
    std::vector<Point> comparison;

    for (int s = 0; s < num_streams; ++s) {
        RandomStreamGen::Config cfg;
        cfg.stream_size = stream_size;
        cfg.duplicate_prob = dup_prob;
        cfg.seed = 1000 + s;
        RandomStreamGen gen(cfg);
        auto stream = gen.generate_stream();
        HyperLogLog hll(B, h);
        std::unordered_set<std::string> exact;
        exact.reserve(stream_size);
        std::size_t cur = 0;
        for (std::size_t ti = 0; ti < T; ++ti) {
            std::size_t target = pref[ti];
            while (cur < target) {
                hll.add(stream[cur]);
                exact.insert(stream[cur]);
                ++cur;
            }
            double trueF0 = static_cast<double>(exact.size());
            double estN = hll.estimate();
            allN[s][ti] = estN;
            if (s == 0) {
                comparison.push_back({ ti, target, trueF0, estN });
            }
        }
    }
    std::vector<double> mean(T, 0.0), sigma(T, 0.0);
    for (std::size_t ti = 0; ti < T; ++ti) {
        double sum = 0.0;
        for (int s = 0; s < num_streams; ++s) sum += allN[s][ti];
        mean[ti] = sum / num_streams;
        double var = 0.0;
        for (int s = 0; s < num_streams; ++s) {
            double d = allN[s][ti] - mean[ti];
            var += d * d;
        }
        var /= (num_streams > 1 ? (num_streams - 1) : 1);
        sigma[ti] = std::sqrt(var);
    }
    {
        std::ofstream out("comparison.csv");
        out << "t,prefix,trueF0,estN\n";
        for (auto& p : comparison) {
            out << p.t << "," << p.prefix << "," << p.trueF0 << "," << p.estN << "\n";
        }
    }
    {
        std::ofstream out("stats.csv");
        out << "t,prefix,mean,sigma,low,high\n";
        for (std::size_t ti = 0; ti < T; ++ti) {
            out << ti << "," << pref[ti] << ","
                << mean[ti] << "," << sigma[ti] << ","
                << (mean[ti] - sigma[ti]) << "," << (mean[ti] + sigma[ti]) << "\n";
        }
    }
    double m = static_cast<double>(1u << B);
    double rse = 1.04 / std::sqrt(m);
    std::cout << "B=" << B << " m=" << (1u << B) << " Theoretical RSE ~ " << rse << " (~" << (rse * 100) << "%)\n";
    std::cout << "Saved comparison.csv and stats.csv\n";
}
