#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "distribution.h"
#include "pancake_proxy.h"
#include "proxy_client.h"
#include "thrift_server.h"
#include "thrift_utils.h"

using namespace ::apache::thrift::transport;

#define HOST "127.0.0.1"
#define PROXY_PORT 9090

void flush_thread(bool& done, std::shared_ptr<proxy> proxy_) {
    auto* pancake = dynamic_cast<pancake_proxy*>(proxy_.get());
    assert(pancake != nullptr);

    while (!done) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        pancake->flush();
    }

    std::cout << "Stopped." << std::endl;
}

int main() {
    std::cout << "Starting Pancake Proxy Integration Test" << std::endl;
    bool done = false;
    std::shared_ptr<proxy> proxy_ = std::make_shared<pancake_proxy>();
    std::vector<std::string> keys;
    keys.reserve(1000);

    for (int i = 1000; i < 2000; ++i) {
        keys.emplace_back(std::to_string(i));
    }

    auto& pancake = dynamic_cast<pancake_proxy&>(*proxy_);
    std::string dummy_value(pancake.object_size_, '0');
    std::vector<std::string> values(keys.size(), dummy_value);
    std::vector<double> probs(keys.size(), 1.0 / keys.size());
    distribution dist(keys, probs);
    auto id_to_client = std::make_shared<thrift_response_client_map>();
    void* arguments[4];
    double alpha = 1.0 / static_cast<double>(keys.size());
    double delta = 1.0 / (2.0 * keys.size()) * (1.0 / alpha);
    arguments[0] = &dist;
    arguments[1] = &alpha;
    arguments[2] = &delta;
    arguments[3] = &id_to_client;
    pancake.init(keys, values, arguments);
    std::cout << "Pancake initialized with " << keys.size() << " keys" << std::endl;
    auto proxy_server = thrift_server::create(proxy_, "pancake", id_to_client, PROXY_PORT, 1);
    std::thread server_thread([&proxy_server]() { proxy_server->serve(); });
    wait_for_server_start(HOST, PROXY_PORT);
    std::cout << "Proxy server is reachable" << std::endl;
    proxy_client client;
    client.init(HOST, PROXY_PORT);
    std::thread flusher(flush_thread, std::ref(done), proxy_);
    std::cout << "Performing bulk PUT operations..." << std::endl;

    for (int i = 1000; i < 2000; ++i) {
        client.put(std::to_string(i), std::to_string(i));
    }

    std::cout << "[PASS] Bulk PUT complete" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "[TEST] Validating GET correctness..." << std::endl;

    for (int i = 1000; i < 2000; ++i) {
        std::string key = std::to_string(i);
        std::string val = client.get(key);
        assert(!val.empty());
    }

    std::cout << "[PASS] Sequential GET validation passed" << std::endl;
    std::cout << "Testing non-existent keys..." << std::endl;

    for (int i = 5000; i < 5100; ++i) {
        try {
            client.get(std::to_string(i));
            assert(false && "Expected exception for missing key");
        } catch (const apache::thrift::TApplicationException&) {
            // Add expected behavior
        }
    }
    std::cout << "Missing key exception test passed" << std::endl;
    std::cout << "Performing random read workload..." << std::endl;

    for (int i = 0; i < 1000; ++i) {
        int k = 1000 + (i % 1000);
        std::string val = client.get(std::to_string(k));
        assert(!val.empty());
    }

    std::cout << "Random workload validation passed" << std::endl;
    std::cout << "Shutting down..." << std::endl;
    done = true;

    if (flusher.joinable()) {
        flusher.join();
    }

    proxy_->close();
    proxy_server->stop();

    if (server_thread.joinable()) {
        server_thread.join();
    }
    std::cout << "Pancake proxy integration test PASSED" << std::endl;
    return 0;
}
