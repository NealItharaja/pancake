#ifndef PANCAKE_QUEUE_H
#define PANCAKE_QUEUE_H

#include <queue>
#include <string>
#include <mutex>
#include <optional>

class queue {
public:
    queue() = default;

    // push a value into the queue
    void push(const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push(value);
    }

    // pop a value from the queue
    std::optional<std::string> pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_.empty()) {
            return std::nullopt;
        }
        std::string value = data_.front();
        data_.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.size();
    }

private:
    std::queue<std::string> data_;
    mutable std::mutex mutex_;
};

#endif // PANCAKE_QUEUE_H
