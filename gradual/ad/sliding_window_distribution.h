#pragma once
#include <cstddef>
#include <deque>
#include "histogram_distribution.h"

namespace ad {

class SlidingWindowDistribution {
public:
    SlidingWindowDistribution(size_t window_size, size_t domain_size);
    size_t observe(size_t key_index);
    size_t slide_one(size_t key_index) { return observe(key_index); }
    const HistogramDistribution& histogram() const noexcept { return histogram_; }
    HistogramDistribution snapshot() const { return histogram_; }
    size_t size() const noexcept { return window_.size(); }
    size_t window_size() const noexcept { return window_size_; }
    size_t domain_size() const noexcept { return histogram_.domain_size(); }
    static constexpr size_t npos = static_cast<size_t>(-1);

private:
    size_t window_size_;
    std::deque<size_t> window_;
    HistogramDistribution histogram_;
};

}
