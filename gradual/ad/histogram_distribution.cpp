#include "histogram_distribution.h"
#include <stdexcept>

namespace ad {

HistogramDistribution::HistogramDistribution(size_t domain_size) : counts_(domain_size, 0), total_(0) {}

void HistogramDistribution::resize(size_t domain_size) {
    counts_.assign(domain_size, 0);
    total_ = 0;
}

void HistogramDistribution::clear() {
    std::fill(counts_.begin(), counts_.end(), 0);
    total_ = 0;
}

void HistogramDistribution::add(size_t key_index) {
    if (key_index >= counts_.size()) {
        throw std::out_of_range("key_index out of domain");
    }

    counts_[key_index]++;
    total_++;
}

void HistogramDistribution::remove(size_t key_index) {
    if (key_index >= counts_.size()) {
        throw std::out_of_range("key_index out of domain");
    }

    if (counts_[key_index] == 0) {
        return;
    }

    counts_[key_index]--;

    if (total_ > 0) {
        total_--;
    }
}

uint64_t HistogramDistribution::count(size_t key_index) const {
    if (key_index >= counts_.size()) {
        throw std::out_of_range("key_index out of domain");
    }

    return counts_[key_index];
}

double HistogramDistribution::probability(size_t key_index) const {
    if (key_index >= counts_.size()) {
        throw std::out_of_range("key_index out of domain");
    }

    if (total_ == 0) {
        return counts_.empty() ? 0.0 : 1.0 / static_cast<double>(counts_.size());
    }

    return static_cast<double>(counts_[key_index]) / static_cast<double>(total_);
}

double HistogramDistribution::cdf(size_t key_index) const {
    if (key_index >= counts_.size()) {
        throw std::out_of_range("key_index out of domain");
    }

    if (total_ == 0) {
        return counts_.empty() ? 0.0 : static_cast<double>(key_index + 1) / static_cast<double>(counts_.size());
    }

    uint64_t prefix = 0;

    for (size_t i = 0; i <= key_index; ++i) {
        prefix += counts_[i];
    }

    return static_cast<double>(prefix) / static_cast<double>(total_);
}

}
