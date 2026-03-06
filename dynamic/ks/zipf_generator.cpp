#include "zipf_generator.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace ks {
ZipfGenerator::ZipfGenerator(double theta, size_t domain_size, uint32_t seed)
    : rng_(seed) {
    if (domain_size == 0) {
        throw std::invalid_argument("domain_size must be > 0");
    }

    if (theta < 0.0) {
        throw std::invalid_argument("theta must be >= 0");
    }

    cdf_.resize(domain_size);
    double sum = 0.0;

    for (size_t i = 1; i <= domain_size; ++i) {
        sum += 1.0 / std::pow(static_cast<double>(i), theta);
        cdf_[i - 1] = sum;
    }

    for (double& x : cdf_) {
        x /= sum;
    }
}

size_t ZipfGenerator::next() {
    const double r = uniform_(rng_);
    auto it = std::lower_bound(cdf_.begin(), cdf_.end(), r);

    if (it == cdf_.end()) {
        return cdf_.size() - 1;
    }

    return static_cast<size_t>(std::distance(cdf_.begin(), it));
}
}
