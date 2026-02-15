#include "distribution.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

Distribution::Distribution(const std::vector<std::string>& values, const std::vector<double>& weights)
    : values_(values), rng_(std::random_device{}()), uni_(0.0, 1.0) {
    if (values.empty() || values.size() != weights.size()) {
        throw std::invalid_argument("Invalid distribution input");
    }
    build_prefix_from_weights(weights);
}

Distribution::Distribution(const std::vector<std::string>& values, double theta)
    : values_(values), rng_(std::random_device{}()), uni_(0.0, 1.0) {
    if (values.empty()) {
        throw std::invalid_argument("Values cannot be empty");
    }

    if (theta < 0.0) {
        throw std::invalid_argument("Theta must be >= 0");
    }
    build_zipf_prefix(theta);
}

void Distribution::build_prefix_from_weights(const std::vector<double>& weights) {
    prefix_sums_.reserve(weights.size());
    total_weight_ = 0.0;

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

void Distribution::build_zipf_prefix(double theta) {
    const size_t N = values_.size();
    prefix_sums_.reserve(N);
    total_weight_ = 0.0;

    for (size_t i = 1; i <= N; ++i) {
        double weight = 1.0 / std::pow(static_cast<double>(i), theta);
        total_weight_ += weight;
        prefix_sums_.push_back(total_weight_);
    }
}

const std::string& Distribution::pick() {
    const double r = uni_(rng_) * total_weight_;
    auto it = std::lower_bound(prefix_sums_.begin(), prefix_sums_.end(), r);
    size_t index = std::distance(prefix_sums_.begin(), it);

    if (index >= values_.size()) {
        return values_.back();
    }
    return values_[index];
}

size_t Distribution::size() const noexcept {
    return values_.size();
}
