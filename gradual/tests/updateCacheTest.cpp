#include "../proxy/updateCache/updateCache.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

void basic_test() {
    std::cout << "Running basic update_cache tests" << std::endl;
    update_cache cache(8);
    std::string key = "1";
    std::string val = "10";
    std::cout << "Check: no update exists initially" << std::endl;
    assert(cache.replica_needs_update(key, 0) == false);
    std::cout << "Check: mark update replicas" << std::endl;
    cache.mark_update(key, val, 8);

    for (size_t i = 0; i < 8; ++i) {
        assert(cache.replica_needs_update(key, i) == true);
    }

    std::cout << "Check: clearing a replica works" << std::endl;
    cache.clear_replica(key, 3);
    assert(cache.replica_needs_update(key, 3) == false);
    assert(cache.replica_needs_update(key, 2) == true);
    std::cout << "Check: resizing replicas" << std::endl;
    cache.resize_replicas(key, 32);
    assert(cache.replica_needs_update(key, 2) == true);
    assert(cache.replica_needs_update(key, 3) == false);
    assert(cache.replica_needs_update(key, 20) == true);
    std::cout << "Basic tests passed" << std::endl;
}

void replica_test() {
    std::cout << "Running replica selection test" << std::endl;
    update_cache cache(16);
    std::string key = "hot_key";
    cache.mark_update(key, "value", 16);

    for (int i = 0; i < 1000; ++i) {
        int r = cache.choose_replica(key, 16, 1, 1.0);
        assert(r >= -1 && r < 16);
    }
    std::cout << "Replica selection test passed" << std::endl;
}

void stress_test() {
    std::cout << "Performing update cache stress test" << std::endl;
    const int START = 1000000;
    const int END   = 2000000;
    const size_t REPLICAS = 64;
    update_cache cache(REPLICAS);
    std::string val = "10";

    for (int i = START; i < END; ++i) {
        std::string key = std::to_string(i);

        assert(cache.replica_needs_update(key, 1) == false);
        cache.mark_update(key, val, REPLICAS);

        for (size_t r = 0; r < 4; ++r) {
            assert(cache.replica_needs_update(key, r) == true);
        }
    }

    for (int i = START; i < START + 100000; ++i) {
        std::string key = std::to_string(i);
        cache.clear_replica(key, 1);
        assert(cache.replica_needs_update(key, 1) == false);
    }
    std::cout << "Memory usage (bytes): " << cache.memory_usage_bytes() << std::endl;
    std::cout << "Stress test passed" << std::endl;
}

int main(int argc, char* argv[]) {
    basic_test();
    replica_test();
    stress_test();
    std::cout << "Passed all update_cache tests :)" << std::endl;
    return 0;
}
