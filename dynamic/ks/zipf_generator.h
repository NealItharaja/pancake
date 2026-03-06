#pragma once
#include <cstddef>
#include <random>
#include <vector>

namespace ks {
struct ZipfParameters {
    double theta = 0.99;
    size_t domain_size = 0;
    uint32_t seed = 7;
};

class ZipfGenerator {
public:
    ZipfGenerator(double theta, size_t domain_size, uint32_t seed = 7);
    explicit ZipfGenerator(const ZipfParameters& params)
        : ZipfGenerator(params.theta, params.domain_size, params.seed) {}
    size_t next();

private:
    std::vector<double> cdf_;
    std::uniform_real_distribution<double> uniform_{0.0, 1.0};
    std::mt19937 rng_;
};
}
