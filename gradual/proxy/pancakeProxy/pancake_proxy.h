#ifndef PANCAKE_PROXY_IMPL_H
#define PANCAKE_PROXY_IMPL_H

#include <string>
#include <vector>
#include <memory>
#include <future>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <random>
#include <utility>
#include "../proxy.h"
#include "../operation/operation.h"
#include "../updateCache/updateCache.h"
#include "../queue/queue.h"
#include "../util/random.h"
#include "../encryption/encryption.h"
#include "../../ad/histogram_distribution.h"
#include "../../ad/sliding_window_distribution.h"
#include "../../storage/interface.h"
#include "../../service/proxy_types.h"

class pancake_proxy : public proxy {
public:
    void init() override { }
    void shutdown() override { close(); }
    std::string get(const std::string &key) override;
    void put(const std::string &key, const std::string &value) override;
    std::vector<std::string> get_batch(const std::vector<std::string> &keys) override;
    void put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values) override;
    void init(const std::vector<std::string> &keys, const std::vector<std::string> &values, void **args);
    void close();
    std::string get(int queue_id, const std::string &key);
    void put(int queue_id, const std::string &key, const std::string &value);
    std::vector<std::string> get_batch(int queue_id, const std::vector<std::string> &keys);
    void put_batch(int queue_id, const std::vector<std::string> &keys, const std::vector<std::string> &values);
    void async_get(const sequence_id &seq_id, const std::string &key);
    void async_put(const sequence_id &seq_id, const std::string &key, const std::string &value);
    void async_get_batch(const sequence_id &seq_id, const std::vector<std::string> &keys);
    void async_put_batch(const sequence_id &seq_id, const std::vector<std::string> &keys, const std::vector<std::string> &values);
    void async_get(const sequence_id &seq_id, int queue_id, const std::string &key);
    void async_put(const sequence_id &seq_id, int queue_id, const std::string &key, const std::string &value);
    void async_get_batch(const sequence_id &seq_id, int queue_id, const std::vector<std::string> &keys);
    void async_put_batch(const sequence_id &seq_id, int queue_id, const std::vector<std::string> &keys, const std::vector<std::string> &values);
    std::string server_host_name_;
    int server_port_ = 0;
    std::string server_type_;
    int server_count_ = 0;
    std::string trace_location_;
    int security_batch_size_ = 0;
    int storage_batch_size_ = 0;
    int object_size_ = 0;

private:
    struct ReplicaSlot {
        size_t key_index = 0;
        size_t replica_id = 0;
    };

    std::string replica_label(const std::string& key, size_t replica_id) const;
    void observe_access(storage_backend& backend, const std::string& key);
    void maybe_run_detection_and_adapt_locked(storage_backend& backend);
    void apply_replica_plan_locked(storage_backend& backend, const std::vector<size_t>& new_replicas);
    std::pair<size_t, size_t> sample_fake_access_locked();
    std::pair<size_t, size_t> sample_real_access_locked();
    void opportunistic_propagate_locked(size_t key_index, size_t replica_id, std::vector<std::pair<std::string, std::string>>& staged_writes);
    std::string read_real_value(storage_backend& backend, size_t key_index, size_t replica_id);
    void run_cover_batch(storage_backend& backend);
    storage_backend& thread_backend();
    void ensure_primary_backend();
    std::shared_ptr<storage_backend> storage_interface_;
    update_cache update_cache_;
    encryption_engine encryption_engine_;
    std::unordered_map<std::string, size_t> key_index_;
    std::vector<std::string> keys_;
    std::vector<size_t> replicas_per_key_;
    std::vector<double> fake_mass_per_key_;
    std::vector<double> fake_cdf_;
    std::vector<double> real_cdf_;
    std::vector<size_t> old_replicas_per_key_;
    std::vector<ReplicaSlot> gain_slots_;
    std::vector<ReplicaSlot> lose_slots_;
    std::unordered_map<std::string, ReplicaSlot> lose_to_gain_slot_;
    std::unordered_set<std::string> pending_gain_labels_;
    bool transition_active_ = false;
    double alpha_threshold_ = 0.0;
    double delta_real_probability_ = 0.5;
    int security_batch_size_effective_ = 3;
    std::unique_ptr<ad::SlidingWindowDistribution> sliding_histogram_;
    ad::HistogramDistribution reference_histogram_;
    std::mt19937 rng_{std::random_device{}()};
    std::mutex dynamic_state_mutex_;
    size_t observe_counter_ = 0;
    bool finished_ = false;
};

#endif // PANCAKE_PROXY_IMPL_H
