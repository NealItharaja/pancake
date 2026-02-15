#ifndef PANCAKE_PROXY_H
#define PANCAKE_PROXY_H

#include <string>
#include <vector>
#include <memory>
#include <future>
#include "../proxy.h"
#include "../operation/operation.h"
#include "../distribution/distribution.h"
#include "../updateCache/updateCache.h"
#include "../queue/queue.h"
#include "../util/random.h"
#include "../encryption/encryption.h"
#include "../../storage/interface.h"

class pancake_proxy : public proxy {
public:
    void init(const std::vector<std::string> &keys, const std::vector<std::string> &values, void **args) override;
    void close() override;
    std::string get(const std::string &key) override;
    void put(const std::string &key, const std::string &value) override;
    std::vector<std::string> get_batch(const std::vector<std::string> &keys) override;
    void put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values) override;
    std::string get(int queue_id, const std::string &key) override;
    void put(int queue_id, const std::string &key, const std::string &value) override;
    std::vector<std::string> get_batch(int queue_id, const std::vector<std::string> &keys) override;
    void put_batch(int queue_id, const std::vector<std::string> &keys, const std::vector<std::string> &values) override;
    void async_get(const sequence_id &seq_id, const std::string &key) override;
    void async_put(const sequence_id &seq_id, const std::string &key, const std::string &value) override;
    void async_get_batch(const sequence_id &seq_id, const std::vector<std::string> &keys) override;
    void async_put_batch(const sequence_id &seq_id, const std::vector<std::string> &keys, const std::vector<std::string> &values) override;
    void async_get(const sequence_id &seq_id, int queue_id, const std::string &key) override;
    void async_put(const sequence_id &seq_id, int queue_id, const std::string &key, const std::string &value) override;
    void async_get_batch(const sequence_id &seq_id, int queue_id, const std::vector<std::string> &keys) override;
    void async_put_batch(const sequence_id &seq_id, int queue_id, const std::vector<std::string> &keys, const std::vector<std::string> &values) override;

private:
    std::shared_ptr<storage_interface> storage_interface_;
    update_cache update_cache_;
    distribution real_distribution_;
    distribution fake_distribution_;
    encryption encryption_engine_;
    bool finished_ = false;
};

#endif // PANCAKE_PROXY_H
