#include "update_cache.h"
#include <random>
#include <algorithm>

update_cache::update_cache(size_t initial_replicas) {
    (void)initial_replicas;
}

size_t update_cache::count_bits(const std::vector<uint8_t>& bits) {
    size_t count = 0;
    for (uint8_t b : bits) {
        count += (b != 0);
    }
    return count;
}

size_t update_cache::leading_zeros(const std::vector<uint8_t>& bits) {
    size_t zeros = 0;
    for (uint8_t b : bits) {
        if (b == 0) ++zeros;
        else break;
    }
    return zeros;
}

bool update_cache::replica_needs_update(
    const std::string& key,
    size_t replica_id
) const {
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

void update_cache::mark_update(
    const std::string& key,
    const std::string& value,
    size_t num_replicas
) {
    std::unique_lock lock(mutex_);

    auto& entry = table_[key];
    entry.value = value;

    if (entry.replica_mask.size() != num_replicas) {
        entry.replica_mask.assign(num_replicas, 1);
    } else {
        std::fill(entry.replica_mask.begin(),
                  entry.replica_mask.end(), 1);
    }
}

void update_cache::clear_replica(
    const std::string& key,
    size_t replica_id
) {
    std::unique_lock lock(mutex_);

    auto it = table_.find(key);
    if (it == table_.end()) return;

    if (replica_id < it->second.replica_mask.size()) {
        it->second.replica_mask[replica_id] = 0;
    }
}

void update_cache::resize_replicas(
    const std::string& key,
    size_t new_size
) {
    std::unique_lock lock(mutex_);

    auto it = table_.find(key);
    if (it == table_.end()) return;

    it->second.replica_mask.resize(new_size, 1);
}

int update_cache::choose_replica(
    const std::string& key,
    size_t num_replicas,
    int frequency,
    double p_max
) const {
    (void)frequency;

    std::shared_lock lock(mutex_);

    auto it = table_.find(key);
    if (it == table_.end() || num_replicas == 0) {
        return -1;
    }

    const auto& mask = it->second.replica_mask;
    std::vector<size_t> candidates;

    for (size_t i = 0; i < num_replicas && i < mask.size(); ++i) {
        if (mask[i]) {
            candidates.push_back(i);
        }
    }

    if (candidates.empty()) {
        return -1;
    }

    std::default_random_engine rng(std::random_device{}());
    std::uniform_real_distribution<double> coin(0.0, 1.0);

    if (coin(rng) > p_max) {
        return -1;
    }

    std::uniform_int_distribution<size_t> pick(0, candidates.size() - 1);
    return static_cast<int>(candidates[pick(rng)]);
}

size_t update_cache::memory_usage_bytes() const {
    std::shared_lock lock(mutex_);

    size_t total = sizeof(*this);

    table_.for_each([&](const auto& kv) {
        total += kv.first.capacity();
        total += kv.second.value.capacity();
        total += kv.second.replica_mask.capacity();
    });

    return total;
}
