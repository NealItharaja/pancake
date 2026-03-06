#pragma once
#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <utility>
#include <vector>

struct ReplicaPlan {
    std::vector<size_t> replicas_per_key;
    std::vector<double> fake_mass_per_key;
    size_t dummy_replicas = 0;
    size_t total_replicas = 0;
    double delta = 0.5;
};

struct ReplicaTransition {
    std::vector<std::pair<size_t, size_t>> gain_slots;
    std::vector<std::pair<size_t, size_t>> lose_slots;
};

class DynamicDistribution {
public:
    explicit DynamicDistribution(size_t domain_size, size_t window_size);
    void observe(size_t key_index);
    std::vector<double> reference_distribution() const;
    std::vector<double> running_distribution() const;
    double ks_statistic() const;
    double ks_threshold(double alpha = 0.05) const;
    bool changed(double alpha = 0.05) const;
    void snapshot_running_as_reference();
    size_t domain_size() const noexcept { return domain_size_; }
    size_t window_size() const noexcept { return window_size_; }
    static ReplicaPlan make_replica_plan(const std::vector<double>& probs, double alpha);
    static ReplicaTransition plan_transition(const ReplicaPlan& old_plan, const ReplicaPlan& new_plan);

private:
    static double normal_quantile_1m_alpha_over_2(double alpha);
    size_t domain_size_;
    size_t window_size_;
    std::deque<size_t> window_;
    std::vector<uint64_t> running_counts_;
    std::vector<uint64_t> reference_counts_;
};
