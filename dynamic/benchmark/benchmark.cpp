#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstring>
#include "timer.h"
#include "distribution.h"
#include "proxy_client.h"
#include "../service/thrift_utils.h"

typedef std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> trace_vector;

static void load_trace(const std::string& trace_location, trace_vector& trace, int client_batch_size) {
    std::ifstream in(trace_location);

    if (!in.is_open()) {
        throw std::runtime_error("Failed to open workload trace: " + trace_location);
    }

    std::vector<std::string> get_keys;
    std::vector<std::string> put_keys;
    std::vector<std::string> put_values;
    std::string line;

    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::string op, key, val;
        iss >> op >> key;

        if (op == "GET") {
            get_keys.push_back(key);

            if ((int)get_keys.size() >= client_batch_size) {
                trace.emplace_back(get_keys, std::vector<std::string>());
                get_keys.clear();
            }
        } else if (op == "PUT") {
            iss >> val;
            put_keys.push_back(key);
            put_values.push_back(val);

            if ((int)put_keys.size() >= client_batch_size) {
                trace.emplace_back(put_keys, put_values);
                put_keys.clear();
                put_values.clear();
            }
        }
    }

    if (!get_keys.empty()) {
        trace.emplace_back(get_keys, std::vector<std::string>());
    }

    if (!put_keys.empty()) {
        trace.emplace_back(put_keys, put_values);
    }

    if (trace.empty()) {
        throw std::runtime_error("Trace loaded but empty.");
    }

    std::cout << "Loaded " << trace.size() << " batched operations." << std::endl;
}

static void run_benchmark(int runtime_seconds, bool collect_stats, std::vector<uint64_t>& latencies_ns, int batch_size, const trace_vector& trace, std::atomic<uint64_t>& throughput, proxy_client& client) {
    using clock = std::chrono::steady_clock;
    const auto start_time = clock::now();
    const auto end_time   = start_time + std::chrono::seconds(runtime_seconds);
    size_t trace_idx = 0;
    uint64_t local_ops = 0;

    while (clock::now() < end_time) {
        const auto& kv = trace[trace_idx];
        auto op_start = clock::now();

        if (kv.second.empty()) {
            client.get_batch(kv.first);
        } else {
            client.put_batch(kv.first, kv.second);
        }

        auto op_end = clock::now();

        if (collect_stats) {
            auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(op_end - op_start).count();
            latencies_ns.push_back(latency / std::max(1, batch_size));
        }
        local_ops += kv.first.size();
        trace_idx = (trace_idx + 1) % trace.size();
    }

    if (collect_stats) {
        throughput += local_ops / runtime_seconds;
    }
}

static void warmup(int batch_size, const trace_vector& trace, proxy_client& client) {
    std::vector<uint64_t> dummy;
    std::atomic<uint64_t> xput{0};
    run_benchmark(10, false, dummy, batch_size, trace, xput, client);
}

static void cooldown(int batch_size, const trace_vector& trace, proxy_client& client) {
    std::vector<uint64_t> dummy;
    std::atomic<uint64_t> xput{0};
    run_benchmark(5, false, dummy, batch_size, trace, xput, client);
}

static void benchmark_client(int client_id, int batch_size, const trace_vector& trace, const std::string& output_dir, const std::string& host, int port, std::atomic<uint64_t>& total_xput) {
    proxy_client client;
    client.init(host, port);
    std::cout << "Client " << client_id << " connected to proxy." << std::endl;
    warmup(batch_size, trace, client);
    std::vector<uint64_t> latencies_ns;
    std::atomic<uint64_t> client_xput{0};
    std::cout << "Client " << client_id << " running benchmark..." << std::endl;
    run_benchmark(30, true, latencies_ns, batch_size, trace, client_xput, client);
    std::string file = output_dir + "/client_" + std::to_string(client_id) + ".txt";
    std::ofstream out(file);

    for (auto l : latencies_ns) {
        out << l << "\n";
    }

    out << "Throughput_ops_per_sec " << client_xput.load() << "\n";
    out.close();
    total_xput += client_xput.load();
    cooldown(batch_size, trace, client);
    std::cout << "Client " << client_id << " finished." << std::endl;
}

static void make_dir(const std::string& path) {
#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    int port = 9090;
    std::string trace_file;
    int num_clients = 1;
    int batch_size = 50;
    std::string output_dir = "benchmark_data";
    int opt;

    while ((opt = getopt(argc, argv, "h:p:t:n:b:o:")) != -1) {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = std::atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'n':
                num_clients = std::atoi(optarg);
                break;
            case 'b':
                batch_size = std::atoi(optarg);
                break;
            case 'o':
                output_dir = optarg;
                break;
            default:
                std::cerr << "Usage: -h host -p port -t trace -n clients -b batch -o outdir\n";
                return 1;
        }
    }

    if (trace_file.empty()) {
        throw std::runtime_error("Trace file is required (-t).");
    }

    make_dir(output_dir);
    trace_vector trace;
    load_trace(trace_file, trace, batch_size);
    std::atomic<uint64_t> total_throughput{0};
    std::vector<std::thread> threads;
    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < num_clients; ++i) {
        threads.emplace_back(benchmark_client, i, batch_size, std::cref(trace), std::cref(output_dir), std::cref(host), port, std::ref(total_throughput));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::steady_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "=====================================\n";
    std::cout << "Total Throughput (ops/sec): " << total_throughput.load() << "\n";
    std::cout << "Total Runtime (s): " << total_time << "\n";
    std::cout << "=====================================\n";
    return 0;
}
