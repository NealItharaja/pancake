#include "benchmark.h"
#include <stdexcept>
#include "sliding_window_distribution.h"
#include "statistical_test.h"
#include "zipf_generator.h"

namespace ks {

BenchmarkResult Benchmark::run(size_t domain_size, size_t window_size, double init_theta, double final_theta, double alpha) {
    if (domain_size == 0 || window_size == 0) {
        throw std::invalid_argument("domain_size and window_size must be > 0");
    }

    ZipfGenerator init_gen(ZipfParameters{init_theta, domain_size, 11});
    ZipfGenerator final_gen(ZipfParameters{final_theta, domain_size, 13});
    SlidingWindowDistribution sliding(window_size, domain_size);

    for (size_t i = 0; i < window_size; ++i) {
        sliding.observe(init_gen.next());
    }

    HistogramDistribution reference = sliding.snapshot();
    BenchmarkResult out{};

    out.threshold = StatisticalTest::threshold(window_size, alpha);

    while (true) {
        sliding.observe(final_gen.next());
        out.num_ops++;
        out.ks_statistic = StatisticalTest::ks_statistic(reference, sliding.histogram());

        if (out.ks_statistic > out.threshold) {
            break;
        }
    }

    return out;
}

}
