#ifndef PANCAKE_PROXY_H
#define PANCAKE_PROXY_H

#include <string>
#include <vector>
#include <memory>

#include "../proxy.h"
#include "../distribution/distribution.h"
#include "../updateCache/updateCache.h"
#include "../queue/queue.h"
#include "../encryption/encryption.h"
#include "../../storage/interface.h"

class pancake_proxy : public proxy {
public:
    void init(const std::vector<std::string>& keys,
              const std::vector<std::string>& values,
              void** args) override;
    void close() override;

    std::string get(const std::string& key) override;
    void put(const std::string& key,
             const std::string& value) override;
    std::vector<std::string> get_batch(
        const std::vector<std::string>& keys) override;
    void put_batch(const std::vector<std::string>& keys,
                   const std::vector<std::string>& values) override;

    std::string get(int queue_id,
                    const std::string& key) override;
    void put(int queue_id,
             const std::string& key,
             const st
