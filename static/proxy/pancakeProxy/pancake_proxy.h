#ifndef PANCAKE_PROXY_IMPL_H
#define PANCAKE_PROXY_IMPL_H

#include <string>
#include <vector>
#include <memory>
#include <future>
#include <mutex>
#include "../proxy.h"
#include "../operation/operation.h"
#include "../distribution/distribution.h"
#include "../updateCache/updateCache.h"
#include "../queue/queue.h"
#include "../util/random.h"
#include "../encryption/encryption.h"
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
    std::shared_ptr<storage_backend> storage_interface_;
    mutable std::mutex storage_mutex_;
    update_cache update_cache_;
    std::unique_ptr<Distribution> real_distribution_;
    std::unique_ptr<Distribution> fake_distribution_;
    encryption_engine encryption_engine_;
    bool finished_ = false;
};

#endif // PANCAKE_PROXY_IMPL_H
