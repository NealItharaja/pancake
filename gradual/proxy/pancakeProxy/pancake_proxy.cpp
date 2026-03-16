#include "pancake_proxy.h"
#include <parallel_hashmap/phmap.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include "../../ad/statistical_test.h"
#include "../../storage/redis/redis.h"

std::string pancake_proxy::replica_label(const std::string& key, size_t replica_id) const {
    return key + "#replica#" + std::to_string(replica_id);
}

std::pair<size_t, size_t> pancake_proxy::sample_real_access_locked() {
    std::uniform_real_distribution<double> unit(0.0, 1.0);
    const double r = unit(rng_);
    auto it = std::lower_bound(real_cdf_.begin(), real_cdf_.end(), r);
    size_t key_idx = static_cast<size_t>(std::distance(real_cdf_.begin(), it));

    if (key_idx >= keys_.size()) {
        key_idx = keys_.size() - 1;
    }

    size_t replica_count = std::max<size_t>(1, replicas_per_key_[key_idx]);
    if (transition_active_ && key_idx < old_replicas_per_key_.size()) {
        replica_count = std::max<size_t>(1, std::min(replica_count, old_replicas_per_key_[key_idx]));
    }

    std::uniform_int_distribution<size_t> rep_dist(0, replica_count - 1);
    return {key_idx, rep_dist(rng_)};
}

std::pair<size_t, size_t> pancake_proxy::sample_fake_access_locked() {
    std::uniform_real_distribution<double> unit(0.0, 1.0);
    const double r = unit(rng_);
    auto it = std::lower_bound(fake_cdf_.begin(), fake_cdf_.end(), r);
    size_t key_idx = static_cast<size_t>(std::distance(fake_cdf_.begin(), it));

    if (key_idx >= keys_.size()) {
        key_idx = keys_.size() - 1;
    }

    size_t replica_count = std::max<size_t>(1, replicas_per_key_[key_idx]);

    if (transition_active_ && key_idx < old_replicas_per_key_.size()) {
        replica_count = std::max<size_t>(1, std::min(replica_count, old_replicas_per_key_[key_idx]));
    }
    std::uniform_int_distribution<size_t> rep_dist(0, replica_count - 1);
    return {key_idx, rep_dist(rng_)};
}

void pancake_proxy::opportunistic_propagate_locked(
    size_t key_index,
    size_t replica_id,
    std::vector<std::pair<std::string, std::string>>& staged_writes) {
    if (key_index >= keys_.size()) {
        return;
    }

    const std::string& key = keys_[key_index];

    if (!update_cache_.replica_needs_update(key, replica_id)) {
        return;
    }

    std::string pending_value;
    if (!update_cache_.try_get_value(key, pending_value)) {
        return;
    }

    staged_writes.emplace_back(replica_label(key, replica_id), pending_value);
    update_cache_.clear_replica(key, replica_id);

    if (transition_active_) {
        const std::string lose_label = replica_label(key, replica_id);
        auto it = lose_to_gain_slot_.find(lose_label);

        if (it != lose_to_gain_slot_.end()) {
            const ReplicaSlot gain = it->second;
            if (gain.key_index < keys_.size()) {
                const std::string gain_key = keys_[gain.key_index];
                std::string gain_value;
                if (update_cache_.try_get_value(gain_key, gain_value)) {
                    staged_writes.emplace_back(lose_label, gain_value);
                    pending_gain_labels_.erase(replica_label(gain_key, gain.replica_id));
                    lose_to_gain_slot_.erase(it);
                }
            }
        }

        if (pending_gain_labels_.empty()) {
            transition_active_ = false;
            gain_slots_.clear();
            lose_slots_.clear();
            lose_to_gain_slot_.clear();
            old_replicas_per_key_ = replicas_per_key_;
            std::cerr << "[gradual][ad] replica transition complete\n";
        }
    }
}

std::string pancake_proxy::read_real_value(storage_backend& backend, size_t key_index, size_t replica_id) {
    if (key_index >= keys_.size()) {
        return {};
    }

    const std::string& key = keys_[key_index];
    auto v = backend.get(replica_label(key, replica_id));
    if (v) {
        return *v;
    }

    v = backend.get(key);
    return v ? *v : std::string{};
}

void pancake_proxy::apply_replica_plan_locked(storage_backend& backend, const std::vector<size_t>& new_replicas) {
    for (size_t i = 0; i < keys_.size(); ++i) {
        const size_t old_count = std::max<size_t>(1, replicas_per_key_[i]);
        const size_t new_count = std::max<size_t>(1, new_replicas[i]);

        if (new_count <= old_count) {
            continue;
        }

        const std::string seed_value = read_real_value(backend, i, 0);
        for (size_t rep = old_count; rep < new_count; ++rep) {
            const std::string label = replica_label(keys_[i], rep);
            if (!backend.exists(label)) {
                backend.put(label, seed_value);
            }
        }
    }

    replicas_per_key_ = new_replicas;
    for (size_t i = 0; i < keys_.size(); ++i) {
        update_cache_.resize_replicas(keys_[i], std::max<size_t>(1, replicas_per_key_[i]));
    }
}

void pancake_proxy::maybe_run_detection_and_adapt_locked(storage_backend& backend) {
    if (observe_counter_ % 4096 != 0 || sliding_histogram_->size() < sliding_histogram_->window_size()) {
        return;
    }

    const double ad_stat = ad::StatisticalTest::anderson_darling_statistic(reference_histogram_, sliding_histogram_->histogram());
    const double threshold = ad::StatisticalTest::threshold(sliding_histogram_->window_size(), 0.05);
    if (ad_stat <= threshold) {
        return;
    }

    std::cerr << "[gradual][ad] distribution change detected: ad=" << ad_stat << " threshold=" << threshold << "\n";
    const size_t n = keys_.size();
    const double alpha_base = 1.0 / static_cast<double>(n);
    std::vector<size_t> new_replicas(n, 1);
    size_t total_replicas = 0;
    double alpha_transition = alpha_base;

    for (size_t i = 0; i < n; ++i) {
        const double p = sliding_histogram_->histogram().probability(i);
        const size_t r = std::max<size_t>(1, static_cast<size_t>(std::ceil(p / alpha_base)));
        new_replicas[i] = r;
        total_replicas += r;
    }

    while (total_replicas > (2 * n)) {
        auto it = std::max_element(new_replicas.begin(), new_replicas.end());
        if (it == new_replicas.end() || *it <= 1) {
            break;
        }
        (*it)--;
        total_replicas--;
    }

    old_replicas_per_key_ = replicas_per_key_;
    gain_slots_.clear();
    lose_slots_.clear();
    lose_to_gain_slot_.clear();
    pending_gain_labels_.clear();
    for (size_t i = 0; i < n; ++i) {
        const size_t old_r = std::max<size_t>(1, old_replicas_per_key_[i]);
        const size_t new_r = std::max<size_t>(1, new_replicas[i]);
        if (new_r > old_r) {
            for (size_t rep = old_r; rep < new_r; ++rep) {
                gain_slots_.push_back({i, rep});
                pending_gain_labels_.insert(replica_label(keys_[i], rep));
            }
        } else if (old_r > new_r) {
            for (size_t rep = new_r; rep < old_r; ++rep) {
                lose_slots_.push_back({i, rep});
            }
        }
    }

    const size_t pair_count = std::min(gain_slots_.size(), lose_slots_.size());
    for (size_t i = 0; i < pair_count; ++i) {
        const ReplicaSlot lose = lose_slots_[i];
        lose_to_gain_slot_[replica_label(keys_[lose.key_index], lose.replica_id)] = gain_slots_[i];
    }

    transition_active_ = !lose_to_gain_slot_.empty();

    for (size_t i = 0; i < n; ++i) {
        const double p = sliding_histogram_->histogram().probability(i);
        const size_t denom_reps = transition_active_ ? std::max<size_t>(1, old_replicas_per_key_[i]) : std::max<size_t>(1, new_replicas[i]);
        const double real_per_replica = p / static_cast<double>(denom_reps);
        alpha_transition = std::max(alpha_transition, real_per_replica);
    }

    alpha_threshold_ = alpha_transition;
    const double denom = std::max(1e-9, 2.0 * static_cast<double>(n) * alpha_threshold_ - 1.0);
    delta_real_probability_ = 1.0 / (2.0 * static_cast<double>(n) * alpha_threshold_);
    delta_real_probability_ = std::clamp(delta_real_probability_, 0.05, 0.95);
    fake_mass_per_key_.assign(n, 0.0);
    real_cdf_.assign(n, 0.0);
    fake_cdf_.assign(n, 0.0);
    double real_acc = 0.0;
    double fake_acc = 0.0;

    for (size_t i = 0; i < n; ++i) {
        const double p = sliding_histogram_->histogram().probability(i);
        const size_t reps_for_real = transition_active_ ? std::max<size_t>(1, old_replicas_per_key_[i]) : std::max<size_t>(1, new_replicas[i]);
        const double real_per_replica = p / static_cast<double>(reps_for_real);
        const double fake_per_replica = std::max(0.0, (alpha_threshold_ - real_per_replica) / denom);
        const double fake_mass = fake_per_replica * static_cast<double>(reps_for_real);
        fake_mass_per_key_[i] = fake_mass;
        real_acc += p;
        fake_acc += fake_mass;
        real_cdf_[i] = real_acc;
        fake_cdf_[i] = fake_acc;
    }

    if (real_acc <= 0.0) {
        const double step = 1.0 / static_cast<double>(n);
        for (size_t i = 0; i < n; ++i) {
            real_cdf_[i] = step * static_cast<double>(i + 1);
        }
    } else {
        for (double& x : real_cdf_) {
            x /= real_acc;
        }
    }

    if (fake_acc <= 0.0) {
        const double step = 1.0 / static_cast<double>(n);
        for (size_t i = 0; i < n; ++i) {
            fake_cdf_[i] = step * static_cast<double>(i + 1);
        }
    } else {
        for (double& x : fake_cdf_) {
            x /= fake_acc;
        }
    }

    apply_replica_plan_locked(backend, new_replicas);
    reference_histogram_ = sliding_histogram_->snapshot();
}

void pancake_proxy::observe_access(storage_backend& backend, const std::string& key) {
    auto it = key_index_.find(key);
    if (it == key_index_.end()) {
        return;
    }

    std::unique_lock<std::mutex> lock(dynamic_state_mutex_, std::try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }
    if (!sliding_histogram_) {
        return;
    }

    sliding_histogram_->observe(it->second);
    observe_counter_++;
    maybe_run_detection_and_adapt_locked(backend);
}

void pancake_proxy::run_cover_batch(storage_backend& backend) {
    std::unique_lock<std::mutex> lock(dynamic_state_mutex_, std::try_to_lock);
    if (!lock.owns_lock() || keys_.empty()) {
        return;
    }

    const int batch = std::max(1, security_batch_size_effective_);
    std::uniform_real_distribution<double> unit(0.0, 1.0);
    std::vector<std::pair<std::string, std::string>> staged_writes;
    std::vector<std::string> staged_reads;
    staged_writes.reserve(static_cast<size_t>(batch));
    staged_reads.reserve(static_cast<size_t>(batch));

    for (int i = 0; i < batch - 1; ++i) {
        const bool choose_real = unit(rng_) < delta_real_probability_;
        auto [key_idx, rep] = choose_real ? sample_real_access_locked() : sample_fake_access_locked();
        opportunistic_propagate_locked(key_idx, rep, staged_writes);
        staged_reads.push_back(replica_label(keys_[key_idx], rep));
    }
    lock.unlock();
    for (const auto& w : staged_writes) {
        backend.put(w.first, w.second);
    }
    for (const auto& r : staged_reads) {
        (void)backend.get(r);
    }
}

void pancake_proxy::ensure_primary_backend() {
    if (storage_interface_) {
        return;
    }

    if (server_type_.empty() || server_type_ == "redis") {
        auto redis = std::make_shared<redis_storage>();
        redis->connect("127.0.0.1", 6379);
        storage_interface_ = redis;
        return;
    }
    throw std::runtime_error("unsupported backend type: " + server_type_);
}

storage_backend& pancake_proxy::thread_backend() {
    thread_local std::shared_ptr<storage_backend> tls_backend;
    if (!tls_backend) {
        if (server_type_.empty() || server_type_ == "redis") {
            auto redis = std::make_shared<redis_storage>();
            redis->connect("127.0.0.1", 6379);
            tls_backend = redis;
        } else {
            throw std::runtime_error("unsupported backend type: " + server_type_);
        }
    }
    return *tls_backend;
}

void pancake_proxy::init(const std::vector<std::string>& keys, const std::vector<std::string>& values, void** args) {
    (void)args;
    finished_ = false;
    ensure_primary_backend();
    if (keys.empty()) {
        throw std::runtime_error("proxy init received empty key domain");
    }

    key_index_.clear();
    keys_ = keys;
    key_index_.reserve(keys.size());

    for (size_t i = 0; i < keys.size(); ++i) {
        key_index_.emplace(keys[i], i);
    }

    replicas_per_key_.assign(keys.size(), 1);
    old_replicas_per_key_ = replicas_per_key_;
    fake_mass_per_key_.assign(keys.size(), 1.0 / static_cast<double>(keys.size()));
    fake_cdf_.assign(keys.size(), 0.0);
    real_cdf_.assign(keys.size(), 0.0);
    const double step = 1.0 / static_cast<double>(keys.size());

    for (size_t i = 0; i < keys.size(); ++i) {
        fake_cdf_[i] = step * static_cast<double>(i + 1);
        real_cdf_[i] = step * static_cast<double>(i + 1);
    }

    alpha_threshold_ = step;
    delta_real_probability_ = 0.5;
    security_batch_size_effective_ = (security_batch_size_ >= 1 && security_batch_size_ <= 8) ? security_batch_size_ : 3;
    const size_t window_size = std::max<size_t>(1024, keys.size());
    sliding_histogram_ = std::make_unique<ad::SlidingWindowDistribution>(window_size, keys.size());
    reference_histogram_ = ad::HistogramDistribution(keys.size());

    for (size_t i = 0; i < window_size; ++i) {
        sliding_histogram_->observe(i % keys.size());
    }

    reference_histogram_ = sliding_histogram_->snapshot();
    observe_counter_ = 0;

    for (size_t i = 0; i < keys.size(); ++i) {
        const std::string value = i < values.size() ? values[i] : std::string{};
        storage_interface_->put(keys[i], value);
        storage_interface_->put(replica_label(keys[i], 0), value);
    }
}

void pancake_proxy::close() {
    finished_ = true;
}

std::string pancake_proxy::get(const std::string& key) {
    auto& backend = thread_backend();
    observe_access(backend, key);

    size_t replica = 0;
    std::vector<std::pair<std::string, std::string>> staged_writes;
    {
        std::lock_guard<std::mutex> lock(dynamic_state_mutex_);
        auto it = key_index_.find(key);
        if (it != key_index_.end()) {
            const size_t idx = it->second;
            size_t replica_count = std::max<size_t>(1, replicas_per_key_[idx]);
            if (transition_active_ && idx < old_replicas_per_key_.size()) {
                replica_count = std::max<size_t>(1, std::min(replica_count, old_replicas_per_key_[idx]));
            }
            std::uniform_int_distribution<size_t> rep_dist(0, replica_count - 1);
            replica = rep_dist(rng_);
            opportunistic_propagate_locked(idx, replica, staged_writes);
        }
    }
    for (const auto& w : staged_writes) {
        backend.put(w.first, w.second);
    }

    auto result = backend.get(replica_label(key, replica));
    if (!result) {
        result = backend.get(key);
    }

    run_cover_batch(backend);
    return result ? *result : std::string{};
}

void pancake_proxy::put(const std::string& key, const std::string& value) {
    auto& backend = thread_backend();
    observe_access(backend, key);
    std::vector<std::pair<std::string, std::string>> staged_writes;

    {
        std::lock_guard<std::mutex> lock(dynamic_state_mutex_);
        auto it = key_index_.find(key);
        if (it != key_index_.end()) {
            const size_t idx = it->second;
            const size_t replica_count = std::max<size_t>(1, replicas_per_key_[idx]);
            size_t write_replica_count = replica_count;
            if (transition_active_ && idx < old_replicas_per_key_.size()) {
                write_replica_count = std::max<size_t>(1, std::min(replica_count, old_replicas_per_key_[idx]));
            }
            std::uniform_int_distribution<size_t> rep_dist(0, write_replica_count - 1);
            const size_t replica = rep_dist(rng_);
            opportunistic_propagate_locked(idx, replica, staged_writes);
            staged_writes.emplace_back(replica_label(key, replica), value);
            staged_writes.emplace_back(key, value);
            update_cache_.mark_update(key, value, replica_count);
            update_cache_.clear_replica(key, replica);
        } else {
            staged_writes.emplace_back(key, value);
        }
    }
    for (const auto& w : staged_writes) {
        backend.put(w.first, w.second);
    }
    run_cover_batch(backend);
}

std::vector<std::string> pancake_proxy::get_batch(const std::vector<std::string>& keys) {
    std::vector<std::string> out;
    out.reserve(keys.size());
    for (const auto& key : keys) {
        out.push_back(get(key));
    }
    return out;
}

void pancake_proxy::put_batch(const std::vector<std::string>& keys, const std::vector<std::string>& values) {
    const size_t n = std::min(keys.size(), values.size());
    for (size_t i = 0; i < n; ++i) {
        put(keys[i], values[i]);
    }
}

std::string pancake_proxy::get(int, const std::string& key) {
    return get(key);
}

void pancake_proxy::put(int, const std::string& key, const std::string& value) {
    put(key, value);
}

std::vector<std::string> pancake_proxy::get_batch(int, const std::vector<std::string>& keys) {
    return get_batch(keys);
}

void pancake_proxy::put_batch(int, const std::vector<std::string>& keys, const std::vector<std::string>& values) {
    put_batch(keys, values);
}

void pancake_proxy::async_get(const sequence_id&, const std::string&) {
    throw std::runtime_error("async_get not implemented (Phase-1)");
}

void pancake_proxy::async_put(const sequence_id&, const std::string&, const std::string&) {
    throw std::runtime_error("async_put not implemented (Phase-1)");
}

void pancake_proxy::async_get_batch(const sequence_id&, const std::vector<std::string>&) {
    throw std::runtime_error("async_get_batch not implemented (Phase-1)");
}

void pancake_proxy::async_put_batch(const sequence_id&, const std::vector<std::string>&, const std::vector<std::string>&) {
    throw std::runtime_error("async_put_batch not implemented (Phase-1)");
}

void pancake_proxy::async_get(const sequence_id&, int, const std::string&) {
    throw std::runtime_error("async_get(queue) not implemented (Phase-1)");
}

void pancake_proxy::async_put(const sequence_id&, int, const std::string&, const std::string&) {
    throw std::runtime_error("async_put(queue) not implemented (Phase-1)");
}

void pancake_proxy::async_get_batch(const sequence_id&, int, const std::vector<std::string>&) {
    throw std::runtime_error("async_get_batch(queue) not implemented (Phase-1)");
}

void pancake_proxy::async_put_batch(const sequence_id&, int, const std::vector<std::string>&, const std::vector<std::string>&) {
    throw std::runtime_error("async_put_batch(queue) not implemented (Phase-1)");
}
