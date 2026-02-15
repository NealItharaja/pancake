#include <cassert>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "distribution.h"

void run_test() Modif{
    std::cout << "Running distribution test\n";
    const size_t N = 1'000'000;
    const double theta = 0.99;
    std::vector<std::string> values;
    values.reserve(N);

    for (size_t i = 0; i < N; ++i) {
        values.emplace_back(std::to_string(i));
    }

    Distribution dist(values, theta);
    std::unordered_map<std::string, size_t> counts;
    const size_t samples = 200000;

    for (size_t i = 0; i < samples; ++i) {
        counts[dist.pick()]++;
    }

    size_t top_count = counts["0"];
    size_t mid_count = counts[std::to_string(N / 2)];
    std::cout << "Top count: " << top_count << "\n";
    std::cout << "Mid count: " << mid_count << "\n";
    assert(top_count > mid_count);
    std::cout << "Test passed \n";
}

int main() {
    run_test();
    return 0;
}
