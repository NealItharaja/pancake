#include "pancake_proxy.h"
#include <parallel_hashmap/phmap.h>
#include <stdexcept>
#include "../../storage/redis/redis.h"

void pancake_proxy::init(const std::vector<std::string> &keys, const std::vector<std::string> &values, void **args) {
    finished_ = false;

    if (!storage_interface_) {
        if (server_type_.empty() || server_type_ == "redis") {
            auto redis = std::make_shared<redis_storage>();
            redis->connect("127.0.0.1", 6379);
            storage_interface_ = redis;
        } else {
            throw std::runtime_error("unsupported backend type: " + server_type_);
        }
    }

    for (size_t i = 0; i < keys.size(); ++i) {
        storage_interface_->put(keys[i], values[i]);
    }
}

void pancake_proxy::close() {
    finished_ = true;
}

std::string pancake_proxy::get(const std::string &key) {
    auto result = storage_interface_->get(key);
    return result ? *result : std::string{};
}

void pancake_proxy::put(const std::string &key, const std::string &value) {
    storage_interface_->put(key, value);
}

std::vector<std::string> pancake_proxy::get_batch(const std::vector<std::string> &keys) {
    auto results = storage_interface_->get_batch(keys);
    std::vector<std::string> out;
    out.reserve(results.size());
    for (const auto &r : results) {
        out.push_back(r ? *r : std::string{});
    }
    return out;
}

void pancake_proxy::put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values) {
    for (size_t i = 0; i < keys.size(); ++i) {
        storage_interface_->put(keys[i], values[i]);
    }
}

std::string pancake_proxy::get(int, const std::string &key) {
    return get(key);
}

void pancake_proxy::put(int, const std::string &key, const std::string &value) {
    put(key, value);
}

std::vector<std::string> pancake_proxy::get_batch(int, const std::vector<std::string> &keys) {
    return get_batch(keys);
}

void pancake_proxy::put_batch(int, const std::vector<std::string> &keys, const std::vector<std::string> &values) {
    put_batch(keys, values);
}

void pancake_proxy::async_get(const sequence_id &, const std::string &) {
    throw std::runtime_error("async_get not implemented (Phase-1)");
}

void pancake_proxy::async_put(const sequence_id &, const std::string &, const std::string &) {
    throw std::runtime_error("async_put not implemented (Phase-1)");
}

void pancake_proxy::async_get_batch(const sequence_id &, const std::vector<std::string> &) {
    throw std::runtime_error("async_get_batch not implemented (Phase-1)");
}

void pancake_proxy::async_put_batch(const sequence_id &, const std::vector<std::string> &, const std::vector<std::string> &) {
    throw std::runtime_error("async_put_batch not implemented (Phase-1)");
}

void pancake_proxy::async_get(const sequence_id &, int, const std::string &) {
    throw std::runtime_error("async_get(queue) not implemented (Phase-1)");
}

void pancake_proxy::async_put(const sequence_id &, int, const std::string &, const std::string &) {
    throw std::runtime_error("async_put(queue) not implemented (Phase-1)");
}

void pancake_proxy::async_get_batch(const sequence_id &, int, const std::vector<std::string> &) {
    throw std::runtime_error("async_get_batch(queue) not implemented (Phase-1)");
}

void pancake_proxy::async_put_batch(const sequence_id &, int, const std::vector<std::string> &, const std::vector<std::string> &) {
    throw std::runtime_error("async_put_batch(queue) not implemented (Phase-1)");
}
