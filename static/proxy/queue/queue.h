#ifndef PANCAKE__H
#define PANCAKE__H

#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>

class queue {
public:
    queue();
    ~queue();
    queue(const queue&) = delete;
    queue& operator=(const queue&) = delete;
    void push(std::string item);
    bool pop(std::string& out);
    void shutdown();
    size_t size() const;
    bool is_closed() const;

private:
    std::<std::string>;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> closed_{false};
};

#endif //PANCAKE__H