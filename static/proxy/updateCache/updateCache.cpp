#include "update_cache.h"
#include <algorithm>
#include <limits>

thread_local std::mt19937 update_cache::rng_(std::random_device{}());

update_cache::update_cache(size_t initial_replicas) {
    (void)initial_replicas;
}

void update_cache::mark_update(const std::string& key, const std::string& value, size_t num_replicas) {
    if (num_replicas == 0) {
        return;
    }

    std::unique_lock lock(mutex_);
    auto& e = table_[key];
    e.value = value;

    if (e.replica_mask.size() != num_replicas) {
        e.replica_mask.assign(num_replicas, 1);
    } else {
        std::fill(e.replica_mask.begin(), e.replica_mask.end(), 1);
    }
}

bool update_cache::replica_needs_update(const std::string& key, size_t replica_id) const {
    std::shared_lock lock(mutex_);
    auto it = table_.find(key);

    if (it == table_.end()) {
        return false;
    }

    const auto& mask = it->second.replica_mask;

    if (replica_id >= mask.size()) {
        return false;
    }
    return mask[replica_id] != 0;
}

void update_cache::clear_replica(const std::string& key, size_t replica_id) {
    std::unique_lock lock(mutex_);
    auto it = table_.find(key);

    if (it == table_.end()) {
        return;
    }

    auto& mask = it->second.replica_mask;

    if (replica_id < mask.size()) {
        mask[replica_id] = 0;
    }
}

void update_cache::resize_replicas(const std::string& key, size_t new_size) {
    std::unique_lock lock(mutex_);
    auto it = table_.find(key);

    if (it == table_.end()) {
        return;
    }

    auto& mask = it->second.replica_mask;
    size_t old_size = mask.size();
    mask.resize(new_size, 1);
    if (new_size > old_size) {}
}

int update_cache::choose_replica(const std::string& key, size_t num_replicas, int frequency, double p_max) const {
    (void)frequency;
    std::shared_lock lock(mutex_);
    auto it = table_.find(key);

    if (it == table_.end() || num_replicas == 0) {
        return -1;
    }

    const auto& mask = it->second.replica_mask;
    size_t limit = std::min(num_replicas, mask.size());
    std::vector<size_t> candidates;
    candidates.reserve(limit);

    for (size_t i = 0; i < limit; ++i) {
        if (mask[i]) {
            candidates.push_back(i);
        }
    }

    if (candidates.empty()) {
        return -1;
    }

    std::uniform_real_distribution<double> prob(0.0, 1.0);

    if (prob(rng_) > p_max) {
        return -1;
    }
    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
    return static_cast<int>(candidates[dist(rng_)]);
}

size_t update_cache::memory_usage_bytes() const {
    std::shared_lock lock(mutex_);
    size_t total = sizeof(*this);
    total += table_.bucket_count() * sizeof(void*);

    for (const auto& [key, entry] : table_) {
        total += sizeof(entry);
        total += key.capacity();
        total += entry.value.capacity();
        total += entry.replica_mask.capacity() * sizeof(uint8_t);
    }
    return total;
}
