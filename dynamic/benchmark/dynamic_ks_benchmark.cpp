#include <cstddef>
#include <iostream>
#include <string>
#include "../ks/benchmark.h"

static void usage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " <domain_size> <window_size> <init_theta> <final_theta> [alpha]\n" << "Example: " << argv0 << " 100000 1000000 0.99 0.60 0.05\n";
}

int main(int argc, char** argv) {
    if (argc < 5 || argc > 6) {
        usage(argv[0]);
        return 1;
    }

    const size_t domain_size = static_cast<size_t>(std::stoull(argv[1]));
    const size_t window_size = static_cast<size_t>(std::stoull(argv[2]));
    const double init_theta = std::stod(argv[3]);
    const double final_theta = std::stod(argv[4]);
    const double alpha = (argc == 6) ? std::stod(argv[5]) : 0.05;

    std::cerr << "Running dynamic KS benchmark...\n";

    const ks::BenchmarkResult result = ks::Benchmark::run(domain_size, window_size, init_theta, final_theta, alpha);

    std::cout << "ks_stat=" << result.ks_statistic << " threshold=" << result.threshold << " num_ops=" << result.num_ops << "\n";
    
    return 0;
}

