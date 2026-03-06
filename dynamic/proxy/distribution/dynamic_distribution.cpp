#include "dynamic_distribution.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace {
double clamp01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}
}

DynamicDistribution::DynamicDistribution(size_t domain_size, size_t window_size)
    : domain_size_(domain_size), window_size_(window_size), running_counts_(domain_size, 0), reference_counts_(domain_size, 0) {
    if (domain_size_ == 0 || window_size_ == 0) {
        throw std::invalid_argument("domain_size and window_size must be > 0");
    }
}

void DynamicDistribution::observe(size_t key_index) {
    if (key_index >= domain_size_) {
        throw std::out_of_range("key_index outside domain");
    }

    window_.push_back(key_index);
    running_counts_[key_index]++;

    if (window_.size() > window_size_) {
        const size_t old = window_.front();
        window_.pop_front();
        running_counts_[old]--;
    }
}

std::vector<double> DynamicDistribution::reference_distribution() const {
    const uint64_t total = std::accumulate(reference_counts_.begin(), reference_counts_.end(), uint64_t{0});
    std::vector<double> probs(domain_size_, 0.0);

    if (total == 0) {
        const double u = 1.0 / static_cast<double>(domain_size_);
        std::fill(probs.begin(), probs.end(), u);
        return probs;
    }

    for (size_t i = 0; i < domain_size_; ++i) {
        probs[i] = static_cast<double>(reference_counts_[i]) / static_cast<double>(total);
    }

    return probs;
}

std::vector<double> DynamicDistribution::running_distribution() const {
    const uint64_t total = std::accumulate(running_counts_.begin(), running_counts_.end(), uint64_t{0});
    std::vector<double> probs(domain_size_, 0.0);

    if (total == 0) {
        const double u = 1.0 / static_cast<double>(domain_size_);
        std::fill(probs.begin(), probs.end(), u);
        return probs;
    }

    for (size_t i = 0; i < domain_size_; ++i) {
        probs[i] = static_cast<double>(running_counts_[i]) / static_cast<double>(total);
    }

    return probs;
}

double DynamicDistribution::ks_statistic() const {
    const uint64_t ref_total = std::accumulate(reference_counts_.begin(), reference_counts_.end(), uint64_t{0});
    const uint64_t run_total = std::accumulate(running_counts_.begin(), running_counts_.end(), uint64_t{0});

    if (ref_total == 0 || run_total == 0) {
        return 0.0;
    }

    double ref_cdf = 0.0;
    double run_cdf = 0.0;
    double sup = 0.0;

    for (size_t i = 0; i < domain_size_; ++i) {
        ref_cdf += static_cast<double>(reference_counts_[i]) / static_cast<double>(ref_total);
        run_cdf += static_cast<double>(running_counts_[i]) / static_cast<double>(run_total);
        sup = std::max(sup, std::abs(ref_cdf - run_cdf));
    }
    return sup;
}

double DynamicDistribution::normal_quantile_1m_alpha_over_2(double alpha) {
    if (alpha <= 0.01) return 1.63;
    if (alpha <= 0.02) return 1.52;
    if (alpha <= 0.05) return 1.36;
    if (alpha <= 0.10) return 1.22;
    return 1.07;
}

double DynamicDistribution::ks_threshold(double alpha) const {
    const double c = normal_quantile_1m_alpha_over_2(alpha);
    const double n = static_cast<double>(std::max<size_t>(1, window_.size()));
    return c * std::sqrt(2.0 / n);
}

bool DynamicDistribution::changed(double alpha) const {
    return ks_statistic() >= ks_threshold(alpha);
}

void DynamicDistribution::snapshot_running_as_reference() {
    reference_counts_ = running_counts_;
}

ReplicaPlan DynamicDistribution::make_replica_plan(const std::vector<double>& probs, double alpha) {
    if (probs.empty()) {
        throw std::invalid_argument("empty probability vector");
    }

    if (alpha <= 0.0) {
        throw std::invalid_argument("alpha must be > 0");
    }

    const size_t n = probs.size();
    ReplicaPlan plan;
    plan.replicas_per_key.resize(n, 1);
    plan.fake_mass_per_key.assign(n, 0.0);
    size_t total_real_replicas = 0;

    for (size_t i = 0; i < n; ++i) {
        const double p = clamp01(probs[i]);
        const size_t r = std::max<size_t>(1, static_cast<size_t>(std::ceil(p / alpha)));
        plan.replicas_per_key[i] = r;
        total_real_replicas += r;
    }

    const size_t total_target = 2 * n;
    plan.dummy_replicas = (total_real_replicas < total_target) ? (total_target - total_real_replicas) : 0;
    plan.total_replicas = total_real_replicas + plan.dummy_replicas;
    const double target_per_replica = 1.0 / static_cast<double>(plan.total_replicas);
    const double delta = 1.0 / (static_cast<double>(plan.total_replicas) * alpha);
    plan.delta = clamp01(delta);
    const double denom = std::max(1e-12, (1.0 - plan.delta));

    for (size_t i = 0; i < n; ++i) {
        const double p = clamp01(probs[i]);
        const double real_mass = plan.delta * p;
        const double target_mass = target_per_replica * static_cast<double>(plan.replicas_per_key[i]);
        plan.fake_mass_per_key[i] = std::max(0.0, (target_mass - real_mass) / denom);
    }

    return plan;
}

ReplicaTransition DynamicDistribution::plan_transition(const ReplicaPlan& old_plan, const ReplicaPlan& new_plan) {
    if (old_plan.replicas_per_key.size() != new_plan.replicas_per_key.size()) {
        throw std::invalid_argument("plan size mismatch");
    }

    ReplicaTransition out;
    const size_t n = old_plan.replicas_per_key.size();

    for (size_t i = 0; i < n; ++i) {
        const size_t old_r = old_plan.replicas_per_key[i];
        const size_t new_r = new_plan.replicas_per_key[i];

        if (new_r > old_r) {
            for (size_t slot = old_r; slot < new_r; ++slot) {
                out.gain_slots.emplace_back(i, slot);
            }
        } else if (old_r > new_r) {
            for (size_t slot = new_r; slot < old_r; ++slot) {
                out.lose_slots.emplace_back(i, slot);
            }
        }
    }
    return out;
}
