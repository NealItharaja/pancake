#include <string>
#include <vector>
#include <random>

class Distribution {
public:
    Distribution(const std::vector<std::string>& values, const std::vector<double>& weights);
    const std::string& pick();
    size_t size() const noexcept;

private:
    void build_prefix_sums();
    std::vector<std::string> values_;
    std::vector<double> prefix_sums_;
    double total_weight_;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> uni_;
};
