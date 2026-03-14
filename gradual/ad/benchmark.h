#pragma once
#include <cstddef>

namespace ad {

struct BenchmarkResult {
    double ad_statistic = 0.0;
    double threshold = 0.0;
    size_t num_ops = 0;
};

class Benchmark {
public:
    static BenchmarkResult run(size_t domain_size, size_t window_size, double init_theta, double final_theta, double alpha = 0.05);
};
}
