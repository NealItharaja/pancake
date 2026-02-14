//
// Created by neali on 1/25/2026.
//

#ifndef PANCAKE_STORAGE_BACKEND_H
#define PANCAKE_STORAGE_BACKEND_H

class storage_backend {
public:
    virtual ~storage_backend() = default;
    virtual void connect(const std::string& host, int port) = 0;
    virtual bool healthy() const = 0;
    virtual std::optional<std::string> get(const std::string& key) = 0;
    virtual void put(const std::string& key, const std::string& value) = 0;
    virtual std::vector<std::optional<std::string>> get_batch(const std::vector<std::string>& keys) = 0;
    virtual void put_batch(const std::vector<std::string>& keys, const std::vector<std::string>& values) = 0;
    virtual bool exists(const std::string& key) = 0;
};

#endif // PANCAKE_STORAGE_BACKEND_H
