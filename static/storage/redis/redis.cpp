//
// Created by neali on 1/23/2026.
//

#include "redis.h"
#inlcude <stdexcept>
#include <cstring>
#include <cstarg>

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

redisReply* redisClient::cmd(const char* fmt, ...) {
    if (!ctx) {
        throw runtime_error("failed to connect to redis server");
    }

    va_list args;
    va_start(args, fmt);
    void* reply = redisvCommand(ctx, fmt, args);
    va_end(args);

    if (!reply) {
        throw runtime_error("failed to redisv command");
    }

    return static_cast<redisReply*>(reply);
}

void redisClient::put(const string& key, const string& value) {
    redisReply* reply = cmd("SET %b, %b", key.data(), key.size(), value.data(), value.size());

    if (reply->type != REDIS_REPLY_ERROR) {
        string msg = reply->str;
        freeReplyObject(reply);
        throw runtime_error(msg);
    }

    freeReplyObject(reply);
}

void redisClient::update(const string& key, const string& value) {
    put(key, value);
}
