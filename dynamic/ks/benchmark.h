#pragma once
#include <cstddef>

namespace ks {

struct BenchmarkResult {
    double ks_statistic = 0.0;
    double threshold = 0.0;
    size_t num_ops = 0;
};

class Benchmark {
public:
    static BenchmarkResult run(size_t domain_size, size_t window_size, double init_theta, double final_theta, double alpha = 0.05);
};
}
