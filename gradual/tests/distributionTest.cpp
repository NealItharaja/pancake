#include <cassert>
#include <iostream>
#include <vector>
#include "../ad/zipf_generator.h"

void run_test() {
    std::cout << "Running zipf generator test\n";
    const size_t N = 10000;
    const double theta = 0.99;
    ad::ZipfGenerator gen(theta, N, 42);
    std::vector<size_t> counts(N, 0);
    const size_t samples = 400000;

    for (size_t i = 0; i < samples; ++i) {
        counts[gen.next()]++;
    }

    const size_t top_count = counts[0];
    const size_t mid_count = counts[N / 2];
    std::cout << "Top count: " << top_count << "\n";
    std::cout << "Mid count: " << mid_count << "\n";
    assert(top_count > mid_count);
    std::cout << "Test passed\n";
}

int main() {
    run_test();
    return 0;
}
