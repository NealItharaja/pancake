//
// Created by neali on 1/23/2026.
//

#include "redis.h"
#include <stdexcept>
#include <cstring>
#include <cstdarg>

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
    redisReply* reply = cmd("SET %b %b", key.data(), key.size(), value.data(), value.size());

    if (reply->type == REDIS_REPLY_ERROR) {
        string msg = reply->str;
        freeReplyObject(reply);
        throw runtime_error(msg);
    }

    freeReplyObject(reply);
}

void redisClient::update(const string& key, const string& value) {
    put(key, value);
}

optional<string> redisClient::get(const string& key) {
    redisReply* reply = cmd("GET %b", key.data(), key.size());

    if (reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(reply);
        return nullopt;
    } else if (reply->type != REDIS_REPLY_STRING) {
        freeReplyObject(reply);
        throw runtime_error("failed to redis get command");
    }

    string value(reply->str, reply->len);
    freeReplyObject(reply);
    return value;
}

vector<string> redisClient::keysScan(const string& key, size_t count) {
    vector<string> result;
    string cursor = "0";
    string pattern = key + "*";

    while (true) {
        redisReply* reply = cmd("SCAN %s MATCH %b COUNT %lu", cursor.c_str(), pattern.data(), pattern.size(), (unsigned long)count);

        if (reply->type != REDIS_REPLY_ARRAY || reply->elements != 2) {
            freeReplyObject(reply);
            throw runtime_error("Redis SCAN unexpected reply format");
        }

        redisReply* cursorReply = reply->element[0];
        redisReply* keysReply   = reply->element[1];

        cursor.assign(cursorReply->str, cursorReply->len);

        if (keysReply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < keysReply->elements; i++) {
                redisReply* k = keysReply->element[i];
                if (k->type == REDIS_REPLY_STRING) {
                    result.emplace_back(k->str, k->len);
                    if (result.size() >= count) {
                        break;
                    }
                }
            }
        }

        freeReplyObject(reply);

        if (cursor == "0" || result.size() >= count) {
            break;
        }
    }

    if (result.size() > count) {
        result.resize(count);
    }

    return result;
}

void redisClient::putBatch(const vector<string> &keys, const vector<string> &values) {
    if (keys.size() != values.size()) {
        throw runtime_error("key vectors must have the same number of elements");
    }

    {
        redisReply* r = cmd("MULTI");
        if (r->type == REDIS_REPLY_ERROR) {
            freeReplyObject(r);
            throw runtime_error("failed to redis multi command");
        }
        freeReplyObject(r);
    }

    for (size_t i = 0; i < keys.size(); i++) {
        redisReply* r = cmd("SET %b %b", keys[i].data(), keys[i].size(), values[i].data(), values[i].size());

        if (r->type == REDIS_REPLY_ERROR) {
            freeReplyObject(r);
            throw runtime_error("failed to redis set command");
        }
        freeReplyObject(r);
    }

    {
        redisReply* r = cmd("EXEC");
        if (r->type == REDIS_REPLY_ERROR) {
            freeReplyObject(r);
            throw runtime_error("failed to redis exec command");
        }
        freeReplyObject(r);
    }
}
