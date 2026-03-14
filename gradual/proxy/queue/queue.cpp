#include "queue.h"

queue::queue() = default;

queue::~queue() {
    shutdown();
}

void queue::push(std::string item) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closed_) {
            return;
        }
        queue_.push(std::move(item));
    }
    cv_.notify_one();
}

bool queue::pop(std::string& out) {
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [&] {
        return closed_ || !queue_.empty();
    });

    if (queue_.empty()) {
        return false;
    }

    out = std::move(queue_.front());
    queue_.pop();
    return true;
}

void queue::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
    }
    cv_.notify_all();
}

size_t queue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

bool queue::is_closed() const {
    return closed_;
}
