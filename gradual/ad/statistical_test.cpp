#include "statistical_test.h"
#include <cmath>
#include <stdexcept>

namespace ad {

static double ad_critical_value(double alpha) {
    if (alpha <= 0.01) {
        return 3.857;
    }

    if (alpha <= 0.025) {
        return 3.070;
    }

    if (alpha <= 0.05) {
        return 2.492;
    }

    if (alpha <= 0.10) {
        return 1.933;
    }

    return 1.226;
}

double StatisticalTest::anderson_darling_statistic(const HistogramDistribution& a, const HistogramDistribution& b) {
    if (a.domain_size() != b.domain_size()) {
        throw std::invalid_argument("domain size mismatch");
    }

    if (a.total() == 0 || b.total() == 0) {
        return 0.0;
    }

    const double n = static_cast<double>(a.total());
    const double m = static_cast<double>(b.total());
    const double scale = (n * m) / (n + m);

    double a_cdf = 0.0;
    double b_cdf = 0.0;
    double pooled_cdf = 0.0;
    double stat_sum = 0.0;

    for (size_t i = 0; i < a.domain_size(); ++i) {
        const double p1 = a.probability(i);
        const double p2 = b.probability(i);
        const double pooled_p = (n * p1 + m * p2) / (n + m);

        a_cdf += p1;
        b_cdf += p2;
        pooled_cdf += pooled_p;

        // Discrete AD integral: weight CDF gap by pooled tail mass.
        const double denom = std::max(1e-12, pooled_cdf * (1.0 - pooled_cdf));
        const double diff = a_cdf - b_cdf;
        stat_sum += (diff * diff) * pooled_p / denom;
    }

    return scale * stat_sum;
}

double StatisticalTest::threshold(size_t sample_size, double alpha) {
    if (sample_size == 0) {
        throw std::invalid_argument("sample_size must be > 0");
    }

    const double c = ad_critical_value(alpha);
    const double n = static_cast<double>(sample_size);
    // Small-sample inflation keeps detector conservative for tiny windows.
    return c * (1.0 + 1.0 / std::sqrt(n));
}

}
