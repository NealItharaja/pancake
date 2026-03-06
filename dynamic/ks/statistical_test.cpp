#include "statistical_test.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace ks {

static double ks_constant(double alpha) {
    if (alpha <= 0.01) {
        return 1.63;
    }

    if (alpha <= 0.02) {
        return 1.52;
    }

    if (alpha <= 0.05) {
        return 1.36;
    }

    if (alpha <= 0.10) {
        return 1.22;
    }

    return 1.07;
}

double StatisticalTest::ks_statistic(const HistogramDistribution& a, const HistogramDistribution& b) {
    if (a.domain_size() != b.domain_size()) {
        throw std::invalid_argument("domain size mismatch");
    }

    if (a.total() == 0 || b.total() == 0) {
        return 0.0;
    }

    double a_cdf = 0.0;
    double b_cdf = 0.0;
    double sup = 0.0;

    for (size_t i = 0; i < a.domain_size(); ++i) {
        a_cdf += a.probability(i);
        b_cdf += b.probability(i);
        sup = std::max(sup, std::abs(a_cdf - b_cdf));
    }

    return sup;
}

double StatisticalTest::threshold(size_t sample_size, double alpha) {
    if (sample_size == 0) {
        throw std::invalid_argument("sample_size must be > 0");
    }

    const double c = ks_constant(alpha);
    const double n = static_cast<double>(sample_size);
    return c * std::sqrt(2.0 / n);
}

}
