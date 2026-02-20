#ifndef PANCAKE_OPERATION_H
#define PANCAKE_OPERATION_H

#include <string>

struct operation {
    std::string key;
    std::string value;

    operation() = default;
    operation(std::string k, std::string v)
        : key(std::move(k)), value(std::move(v)) {}

    bool operator==(const operation& other) const {
        return key == other.key && value == other.value;
    }

    bool operator!=(const operation& other) const {
        return !(*this == other);
    }
};

#endif
