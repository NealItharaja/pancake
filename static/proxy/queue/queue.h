#ifndef PANCAKE_QUEUE_H
#define PANCAKE_QUEUE_H

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

class queue {
public:
    queue();
    ~queue();

    // Push an item into the queue
    void push(std::string item);

    // Pop an item; blocks until item available or queue is closed
    // Returns false if the queue is closed and empty
    bool pop(std::string& out);

    // Close the queue and wake all waiting threads
    void shutdown();

    // Number of items currently in the queue
    size_t size() const;

    // Whether shutdown() has been called
    bool is_closed() const;

private:
    std::queue<std::string> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool closed_ = false;
};

#endif // PANCAKE_QUEUE_H
