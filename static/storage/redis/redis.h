//
// Created by neali on 1/23/2026.
//

#ifndef PANCAKE_REDIS_H
#define PANCAKE_REDIS_H

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <hiredis/hiredis.h>

using namespace std;

class redisClient {
    public:
        redisClient(const string& host, int port);
        ~redisClient();
        redisClient(const redisClient&) = delete;
        redisClient& operator=(const redisClient&) = delete;
        redisClient(redisClient&&) noexcept;
        redisClient& operator=(redisClient&&) noexcept;
        void put(const string& key, const string& value);
        optional<string> get(const string& key);
        vector<string> keysScan(const string& key, size_t count);
        void update(const string& key, const string& value);
		void putBatch(const vector<string> &keys, const vector<string> &values);
    private:
        redisContext* ctx = nullptr;
        redisReply* cmd(const char* fmt, ...);
};

#endif //PANCAKE_REDIS_H