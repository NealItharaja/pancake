#ifndef KV_STORE_H
#define KV_STORE_H
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <parallel_hashmap/phmap.h>
class KVStore {
public:
    KVStore() = default;

    void put(const std::string& key, const std::string& value) {
        map_.insert_or_assign(key, value);
    }

    std::optional<std::string> get(const std::string& key) const {
        auto it = map_.find(key);
        if (it == map_.end()) return std::nullopt;
        return it->second;
    }

    std::vector<std::string> get_batch(const std::vector<std::string>& keys) const {
        std::vector<std::string> out;
        out.reserve(keys.size());
        for (const auto& k : keys) {
            auto v = get(k);
            out.push_back(v ? *v : std::string{});
        }
        return out;
    }

    void put_batch(const std::vector<std::string>& keys,
                   const std::vector<std::string>& values) {
        const size_t n = (keys.size() < values.size()) ? keys.size() : values.size();
        for (size_t i = 0; i < n; i++) {
            put(keys[i], values[i]);
        }
    }

private:

    phmap::parallel_flat_hash_map<std::string, std::string> map_{8};
};

#endif
