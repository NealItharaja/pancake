#include "distribution.h"
#include <algorithm>
#include <stdexcept>

Distribution::Distribution(const std::vector<std::string>& values, const std::vector<double>& weights)
    : values_(values), total_weight_(0.0), rng_(std::random_device{}()), uni_(0.0, 1.0) {

    if (values.empty() || values.size() != weights.size()) {
        throw std::invalid_argument("Invalid distribution input");
    }

    prefix_sums_.reserve(weights.size());

    for (double w : weights) {
        if (w < 0.0) {
            throw std::invalid_argument("Negative weight not allowed");
        }
        total_weight_ += w;
        prefix_sums_.push_back(total_weight_);
    }

    if (total_weight_ <= 0.0) {
        throw std::invalid_argument("Total weight must be > 0");
    }
}

const std::string& Distribution::pick() {
    const double r = uni_(rng_) * total_weight_;

    auto it = std::lower_bound(
        prefix_sums_.begin(),
        prefix_sums_.end(),
        r
    );

    size_t index = std::distance(prefix_sums_.begin(), it);

    if (index >= values_.size()) {
        return values_.back();
    }
    return values_[index];
}

size_t Distribution::size() const noexcept {
    return values_.size();
}
