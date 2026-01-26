//
// Created by neali on 1/23/2026.
//

#include "redis.h"
#inlcude <stdexcept>
#include <cstring>

redisClient::redisClient(const string& host, int port) {
    ctx = redisConnect(host.c_str(), port);

    if (!ctx) {
        throw runtime_error("failed to connect to redis server");
    }

    if (ctx->err) {
        string msg = ctx->errstr;
        redisFree(ctx);
        ctx = nullptr;
        throw runtime_error(msg);
    }
}

redisClient::~redisClient() {
    if (ctx) {
        redisFree(ctx);
        ctx = nullptr;
    }
}

redisClient::redisClient(redisClient&& other) noexcept {
    ctx = other.ctx;
    other.ctx = nullptr;
}

redisClient& redisClient::operator=(redisClient&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    if (ctx) {
        redisFree(ctx);
    }

    ctx = other.ctx;
    other.ctx = nullptr;
    return *this;
}
