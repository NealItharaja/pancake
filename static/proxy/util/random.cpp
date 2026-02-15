#include "random.h"
#include <random>
#include <stdexcept>

namespace util {
    static thread_local std::mt19937 rng {std::random_device{}()};

    uint32_t random_uint(uint32_t min, uint32_t max) {
        if (min > max) {
            throw std::invalid_argument("random_uint: min > max");
        }
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(rng);
    }

    std::string random_numeric_string(std::size_t length) {
        static constexpr char digits[] = "0123456789";
        static constexpr std::size_t digits_len = sizeof(digits) - 1;

        std::string out;
        out.reserve(length);

        for (std::size_t i = 0; i < length; ++i) {
            out.push_back(digits[random_uint(0, digits_len - 1)]);
        }
        return out;
    }

    bool chance(double p) {
        if (p <= 0.0) {
            return false;
        }

        if (p >= 1.0) {
            return true;
        }

        std::bernoulli_distribution dist(p);
        return dist(rng);
    }
}
