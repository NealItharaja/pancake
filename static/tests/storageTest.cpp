#include "redis.h"
#include "storage_backend.h"
#include <iostream>
#include <cassert>
#include <random>
#include <chrono>
#include <memory>

using namespace std;
using namespace std::chrono;

static string random_string(size_t length) {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    static thread_local mt19937 gen(random_device{}());
    uniform_int_distribution<> dist(0, sizeof(charset) - 2);
    string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i)
        result += charset[dist(gen)];
    return result;
}

void basic_test(shared_ptr<storage_backend> store) {
    cout << "Running basic Redis storage test..." << endl;
    assert(store->healthy());
    string key = "pancake:key:1";
    string value = "value_1";
    store->put(key, value);
    auto result = store->get(key);
    assert(result.has_value());
    assert(result.value() == value);
    auto missing = store->get("pancake:missing:key");
    assert(!missing.has_value());
    cout << "Basic functional test passed." << endl;
}

void batch_test(shared_ptr<storage_backend> store, size_t n = 1000) {
    cout << "Running batch test (" << n << " keys)..." << endl;
    vector<string> keys;
    vector<string> values;
    keys.reserve(n);
    values.reserve(n);

    for (size_t i = 0; i < n; ++i) {
        keys.emplace_back("pancake:batch:key:" + to_string(i));
        values.emplace_back("val_" + to_string(i));
    }

    store->put_batch(keys, values);
    auto results = store->get_batch(keys);
    assert(results.size() == n);

    for (size_t i = 0; i < n; ++i) {
        assert(results[i].has_value());
        assert(results[i].value() == values[i]);
    }
    cout << "Batch test passed." << endl;
}

void stress_test(shared_ptr<storage_backend> store, size_t operations = 50000) {
    cout << "Running YCSB-style Redis stress test..." << endl;
    const size_t value_size = 1000;
    vector<string> keys;
    vector<string> values;
    keys.reserve(operations);
    values.reserve(operations);

    for (size_t i = 0; i < operations; ++i) {
        keys.emplace_back("ycsb:key:" + to_string(i));
        values.emplace_back(random_string(value_size));
    }

    auto start = high_resolution_clock::now();
    store->put_batch(keys, values);
    auto results = store->get_batch(keys);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();

    for (size_t i = 0; i < operations; ++i) {
        assert(results[i].has_value());
        assert(results[i].value() == values[i]);
    }

    cout << "Operations: " << operations << endl;
    cout << "Total time: " << duration << " ms" << endl;
    cout << "Throughput: " << (operations * 1000.0 / duration) << " ops/sec" << endl;
    cout << "YCSB-style stress test passed." << endl;
}

int main() {
    cout << "=== Pancake Redis Storage Test ===" << endl;
    auto store = make_shared<redis_storage>();
    store->connect("127.0.0.1", 6379);

    if (!store->healthy()) {
        cerr << "Redis is not healthy. Is the server running?" << endl;
        return 1;
    }

    basic_test(store);
    batch_test(store, 5000);
    stress_test(store, 50000);
    cout << "All Redis storage tests passed successfully." << endl;
    return 0;
}
