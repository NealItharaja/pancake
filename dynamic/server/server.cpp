#include <parallel_hashmap/phmap.h>
#include <atomic>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#define usleep(x) Sleep((x) / 1000)
#else
#include <unistd.h>
#endif

#include "distribution.h"
#include "pancake_proxy.h"
#include "../proxy/thrift_server.h"
#include "../service/thrift_utils.h"

#define HOST "127.0.0.1"
#define PROXY_PORT 9090

using trace_vector = std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>;

Distribution load_trace(const std::string& trace_path, trace_vector& trace, int batch_size) {
    phmap::flat_hash_map<std::string, int> freq;
    int total = 0;
    std::ifstream in(trace_path);

    if (!in.is_open()) {
        std::perror("Failed to open workload file");
        std::exit(EXIT_FAILURE);
    }

    std::vector<std::string> get_keys;
    std::vector<std::string> put_keys;
    std::vector<std::string> put_vals;
    std::string line;

    while (std::getline(in, line)) {
        auto space = line.find(' ');
        auto key = line.substr(space + 1);
        std::string val;

        if (auto vpos = key.find(' '); vpos != std::string::npos) {
            val = key.substr(vpos + 1);
            key = key.substr(0, vpos);
        }

        if (val.empty()) {
            get_keys.push_back(key);

            if ((int)get_keys.size() == batch_size) {
                trace.emplace_back(get_keys, std::vector<std::string>{});
                get_keys.clear();
            }
        } else {
            put_keys.push_back(key);
            put_vals.push_back(val);

            if ((int)put_keys.size() == batch_size) {
                trace.emplace_back(put_keys, put_vals);
                put_keys.clear();
                put_vals.clear();
            }
        }
        freq[key]++;
        total++;
    }

    if (!get_keys.empty()) {
        trace.emplace_back(get_keys, std::vector<std::string>{});
    }
    if (!put_keys.empty()) {
        trace.emplace_back(put_keys, put_vals);
    }

    std::vector<std::string> keys;
    std::vector<double> weights;

    for (auto& [k, v] : freq) {
        keys.push_back(k);
        weights.push_back(static_cast<double>(v) / total);
    }
    return Distribution(keys, weights);
}


void usage() {
    std::cout << "Pancake proxy server\n" << "  -h host\n" << "  -p port\n" << "  -s backend type\n" << "  -n backend count\n" << "  -l workload file\n" << "  -b security batch size\n" << "  -c storage batch size\n" << "  -v value size\n" << "  -q client batch size\n";
}

int main(int argc, char** argv) {
    int client_batch = 50;
    int value_size = 1000;
    auto proxy = std::make_shared<pancake_proxy>();
    int opt;

    while ((opt = getopt(argc, argv, "h:p:s:n:l:b:c:v:q:")) != -1) {
        switch (opt) {
            case 'h':
                proxy->server_host_name_ = optarg;
                break;
            case 'p':
                proxy->server_port_ = std::atoi(optarg);
                break;
            case 's':
                proxy->server_type_ = optarg;
                break;
            case 'n':
                proxy->server_count_ = std::atoi(optarg);
                break;
            case 'l':
                proxy->trace_location_ = optarg;
                break;
            case 'b':
                proxy->security_batch_size_ = std::atoi(optarg);
                break;
            case 'c':
                proxy->storage_batch_size_ = std::atoi(optarg);
                break;
            case 'v':
                proxy->object_size_ = std::atoi(optarg);
                break;
            case 'q':
                client_batch = std::atoi(optarg);
                break;
            default:
                usage();
                return EXIT_FAILURE;
        }
    }

    assert(!proxy->trace_location_.empty());
    trace_vector trace;
    auto dist = load_trace(proxy->trace_location_, trace, client_batch);
    auto items = dist.get_items();
    double alpha = 1.0 / items.size();
    double delta = 1.0 / (2 * items.size()) * 1 / alpha;
    auto response_map = std::make_shared<thrift_response_client_map>();
    void* args[4] = {&dist, &alpha, &delta, &response_map};
    std::string dummy(value_size, '0');
    std::cout << "Initializing proxy...\n";
    proxy->init(items, std::vector<std::string>(items.size(), dummy), args);
    std::cout << "Proxy initialized\n";
    auto server = thrift_server::create(proxy, "pancake", response_map, PROXY_PORT, 1);
    std::thread server_thread([&] { server->serve(); });
    wait_for_server_start(HOST, PROXY_PORT);
    std::cout << "Proxy server running\n";
    server_thread.join();
    return 0;
}
