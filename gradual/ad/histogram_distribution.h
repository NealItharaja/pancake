#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ad {

class HistogramDistribution {
public:
    explicit HistogramDistribution(size_t domain_size = 0);
    void resize(size_t domain_size);
    void clear();
    void add(size_t key_index);
    void remove(size_t key_index);
    uint64_t count(size_t key_index) const;
    uint64_t total() const noexcept { return total_; }
    size_t domain_size() const noexcept { return counts_.size(); }
    double probability(size_t key_index) const;
    double cdf(size_t key_index) const;
    const std::vector<uint64_t>& counts() const noexcept { return counts_; }

private:
    std::vector<uint64_t> counts_;
    uint64_t total_ = 0;
};

}

