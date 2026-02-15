#include "redis.h"
#include <stdexcept>
#include <cstring>
#include <cstdarg>

redis_storage::redis_storage() = default;

redis_storage::~redis_storage() {
    if (ctx) {
        redisFree(ctx);
        ctx = nullptr;
    }
}

redis_storage::redis_storage(redis_storage&& other) noexcept {
    ctx = other.ctx;
    other.ctx = nullptr;
}

redis_storage& redis_storage::operator=(redis_storage&& other) noexcept {
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

void redis_storage::connect(const string& host, int port) {
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

bool redis_storage::healthy() const {
    return ctx != nullptr && ctx->err == 0;
}

redisReply* redis_storage::cmd(const char* fmt, ...) {
    if (!ctx) {
        throw runtime_error("redis not connected");
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

void redis_storage::put(const string& key, const string& value) {
    redisReply* reply = cmd("SET %b %b", key.data(), key.size(), value.data(), value.size());

    if (reply->type == REDIS_REPLY_ERROR) {
        string msg = reply->str;
        freeReplyObject(reply);
        throw runtime_error(msg);
    }

    freeReplyObject(reply);
}

optional<string> redis_storage::get(const string& key) {
    redisReply* reply = cmd("GET %b", key.data(), key.size());

    if (reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(reply);
        return nullopt;
    }

    if (reply->type != REDIS_REPLY_STRING) {
        freeReplyObject(reply);
        throw runtime_error("failed to redis get command");
    }

    string value(reply->str, reply->len);
    freeReplyObject(reply);
    return value;
}

vector<optional<string>> redis_storage::get_batch(const vector<string>& keys) {
    vector<optional<string>> results;
    results.reserve(keys.size());

    for (const auto& key : keys) {
        results.emplace_back(get(key));
    }
    return results;
}

void redis_storage::put_batch(const vector<string>& keys, const vector<string>& values) {
    if (keys.size() != values.size()) {
        throw runtime_error("key/value vector size mismatch");
    }

    {
        redisReply* r = cmd("MULTI");
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
        freeReplyObject(r);
    }
}

vector<string> redis_storage::keysScan(const string& key, size_t count) {
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

bool redis_storage::exists(const string& key) {
    redisReply* reply = cmd("EXISTS %b", key.data(), key.size());

    if (reply->type != REDIS_REPLY_INTEGER) {
        freeReplyObject(reply);
        throw runtime_error("EXISTS command failed");
    }

    bool exists = (reply->integer == 1);
    freeReplyObject(reply);
    return exists;
}
