#ifndef PANCAKE_UTIL_H
#define PANCAKE_UTIL_H

#include <cstdint>
#include <string>

namespace util {
    uint32_t random_uint(uint32_t min, uint32_t max);
    std::string random_numeric_string(std::size_t length);
    bool chance(double p);

}

#endif
