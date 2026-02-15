#pragma once
#include <vector>
#include <string>
#include <random>

class Distribution {
public:
    Distribution(const std::vector<std::string>& values, const std::vector<double>& weights);
    Distribution(const std::vector<std::string>& values, double theta);
    const std::string& pick();
    size_t size() const noexcept;

private:
    std::vector<std::string> values_;
    std::vector<double> prefix_sums_;
    double total_weight_;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> uni_;
    void build_prefix_from_weights(const std::vector<double>& weights);
    void build_zipf_prefix(double theta);
};
