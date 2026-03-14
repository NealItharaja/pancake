#include "encryption.h"
#include <iostream>
#include <cassert>
#include <random>
#include <chrono>
#include <vector>

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

void correctness_test() {
    cout << "Running correctness tests..." << endl;
    encryption_engine engine;
    string plain = "pancake_test_string";
    string encrypted = engine.encrypt(plain);
    string decrypted = engine.decrypt(encrypted);

    assert(plain == decrypted);
    assert(encrypted != plain);

    string mac1 = engine.hmac("test_key");
    string mac2 = engine.hmac("test_key");
    
    assert(mac1 == mac2);

    cout << "Correctness tests passed." << endl;
}

void stress_test(size_t operations) {
    cout << "\nRunning stress test..." << endl;

    encryption_engine engine;
    vector<string> values;
    values.reserve(operations);

    for (size_t i = 0; i < operations; ++i) {
        values.push_back(random_string(1000));
    }

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < operations; ++i) {
        string encrypted = engine.encrypt(values[i]);
        string decrypted = engine.decrypt(encrypted);
        assert(decrypted == values[i]);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();

    cout << "Operations: " << operations << endl;
    cout << "Total time: " << duration << " ms" << endl;
    cout << "Throughput: " << (operations * 1000.0 / duration) << " ops/sec" << endl;
    cout << "YCSB stress test passed." << endl;
}

int main() {
    cout << "=== Pancake Encryption Engine Test ===\n" << endl;

    correctness_test();
    stress_test(100000);

    cout << "\nAll tests passed successfully.\n" << endl;

    return 0;
}
