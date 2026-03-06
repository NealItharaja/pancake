#include "sliding_window_distribution.h"
#include <stdexcept>

namespace ks {

SlidingWindowDistribution::SlidingWindowDistribution(size_t window_size, size_t domain_size)
    : window_size_(window_size), histogram_(domain_size) {
    if (window_size_ == 0 || domain_size == 0) {
        throw std::invalid_argument("window_size and domain_size must be > 0");
    }
}

size_t SlidingWindowDistribution::observe(size_t key_index) {
    histogram_.add(key_index);
    window_.push_back(key_index);

    if (window_.size() <= window_size_) {
        return npos;
    }

    const size_t removed = window_.front();
    window_.pop_front();
    histogram_.remove(removed);
    return removed;
}

}
