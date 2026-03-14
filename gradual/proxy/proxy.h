#ifndef PANCAKE_PROXY_H
#define PANCAKE_PROXY_H

#include <string>
#include <vector>

class proxy {
public:
    virtual ~proxy() = default;
    virtual void init() = 0;
    virtual void shutdown() = 0;
    virtual std::string get(const std::string& key) = 0;
    virtual void put(const std::string& key, const std::string& value) = 0;
    virtual std::vector<std::string> get_batch(const std::vector<std::string>& keys) = 0;
    virtual void put_batch(const std::vector<std::string>& keys, const std::vector<std::string>& values) = 0;
};

#endif
