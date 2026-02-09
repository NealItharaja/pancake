#ifndef PANCAKE_ASYNC_CLIENT_H
#define PANCAKE_ASYNC_CLIENT_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include "client.h"
#include "pancake_thrift.h"
#include "thrift_response_service.h"
#include "queue.h"
#include "command_response_reader.h"

class async_client final : public client {
public:
    async_client() = default;
    ~async_client() override;
    async_client(const async_client&) = delete;
    async_client& operator=(const async_client&) = delete;
    void init(const std::string& host, int port) override;
    std::string get(const std::string& key) override;
    void put(const std::string& key, const std::string& value) override;
    std::vector<std::string> get_batch(const std::vector<std::string>& keys) override;
    void put_batch(const std::vector<std::string>& keys, const std::vector<std::string>& values) override;
    int64_t client_id() const;
    int completed_requests() const;
    void shutdown();

private:
    enum class op_type : int {
        GET = 0,
        PUT = 1,
        GET_BATCH = 2,
        PUT_BATCH = 3
    };

    int64_t acquire_client_id();
    void response_loop();
    std::shared_ptr<apache::thrift::transport::TTransport> transport_;
    std::shared_ptr<pancake::pancake_thriftClient> client_;
    queue<op_type> inflight_;
    std::atomic<int> completed_{0};
    std::atomic<bool> stop_{false};
    std::mutex mtx_;
    std::condition_variable cv_;
    int in_flight_limit_ = 16;
    int sequence_no_ = 0;
    int64_t client_id_ = -1;
    pancake::sequence_id seq_id_;
    command_response_reader reader_;
    std::thread response_thread_;
};

#endif //PANCAKE_ASYNC_CLIENT_H