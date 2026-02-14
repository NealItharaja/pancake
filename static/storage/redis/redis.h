//
// Created by neali on 1/23/2026.
//

#ifndef PANCAKE_REDIS_H
#define PANCAKE_REDIS_H

#include "../interface.h"
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <hiredis/hiredis.h>

using namespace std;

class redis_storage : public storage_backend {
public:
    redis_storage();
    ~redis_storage();
    redis_storage(const redis_storage&) = delete;
    redis_storage& operator=(const redis_storage&) = delete;
    redis_storage(redis_storage&&) noexcept;
    redis_storage& operator=(redis_storage&&) noexcept;
    void connect(const string& host, int port) override;
    optional<string> get(const string& key) override;
    void put(const string& key, const string& value) override;
    vector<optional<string>> get_batch(const vector<string>& keys) override;
    void put_batch(const vector<string>& keys, const vector<string>& values) override;
    bool healthy() const override;
    vector<string> keysScan(const string& key, size_t count);
    bool exists(const std::string& key) override;

private:
    redisContext* ctx = nullptr;
    redisReply* cmd(const char* fmt, ...);
};
#endif // PANCAKE_REDIS_H
