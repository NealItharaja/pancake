#ifndef PANCAKE_TIMER_H
#define PANCAKE_TIMER_H

#include <chrono>
#include <cstdint>

class Timer {
public:
    using clock = std::chrono::steady_clock;
    Timer() : start_(clock::now()) {}
    void reset() { start_ = clock::now(); }
    uint64_t elapsed_ns() const { return std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - start_).count(); }
    uint64_t elapsed_us() const { return std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - start_).count(); }
    uint64_t elapsed_ms() const { return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start_).count(); }

private:
    clock::time_point start_;
};

#endif // PANCAKE_TIMER_H
