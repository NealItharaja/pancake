#include "async_client.h"

using apache::thrift::protocol::TBinaryProtocol;
using apache::thrift::transport::TSocket;
using apache::thrift::transport::TFramedTransport;

async_client::~async_client() {
    shutdown();
}

void async_client::init(const std::string& host, int port) {
    auto socket = std::make_shared<TSocket>(host, port);
    socket->setRecvTimeout(10000);
    socket->setSendTimeout(1200000);

    transport_ = std::make_shared<TFramedTransport>(socket);
    auto protocol = std::make_shared<TBinaryProtocol>(transport_);

    client_ = std::make_shared<pancake::pancake_thriftClient>(protocol);
    transport_->open();

    client_id_ = acquire_client_id();

    seq_id_.__set_client_id(client_id_);

    reader_ = command_response_reader(protocol);
    response_thread_ = std::thread(&async_client::response_loop, this);
}

int64_t async_client::acquire_client_id() {
    auto id = client_->get_client_id();
    client_->register_client_id(1, id);
    return id;
}

int64_t async_client::client_id() const {
    return client_id_;
}

std::string async_client::get(const std::string& key) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [&] { return inflight_.size() < in_flight_limit_; });

    seq_id_.__set_client_seq_no(sequence_no_++);
    client_->async_get(seq_id_, key);
    inflight_.push(op_type::GET);

    return {};
}

void async_client::put(const std::string& key, const std::string& value) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [&] { return inflight_.size() < in_flight_limit_; });

    seq_id_.__set_client_seq_no(sequence_no_++);
    client_->async_put(seq_id_, key, value);
    inflight_.push(op_type::PUT);
}

std::vector<std::string>
async_client::get_batch(const std::vector<std::string>& keys) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [&] { return inflight_.size() < in_flight_limit_; });

    seq_id_.__set_client_seq_no(sequence_no_++);
    client_->async_get_batch(seq_id_, keys);
    inflight_.push(op_type::GET_BATCH);

    return {};
}

void async_client::put_batch(const std::vector<std::string>& keys,
                                   const std::vector<std::string>& values) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [&] { return inflight_.size() < in_flight_limit_; });

    seq_id_.__set_client_seq_no(sequence_no_++);
    client_->async_put_batch(seq_id_, keys, values);
    inflight_.push(op_type::PUT_BATCH);
}

void async_client::response_loop() {
    std::vector<std::string> response;

    while (!stop_.load()) {
        auto op = inflight_.pop();
        cv_.notify_one();

        try {
            reader_.recv_response(response);
            completed_ += static_cast<int>(response.size());
        } catch (...) {
            // transport closed or shutdown
        }

        response.clear();
    }
}

int async_client::completed_requests() const {
    return completed_.load();
}

void async_client::shutdown() {
    if (stop_.exchange(true)) {
        return;
    }

    inflight_.push(op_type::GET); // unblock pop
    if (response_thread_.joinable()) {
        response_thread_.join();
    }

    if (transport_ && transport_->isOpen()) {
        transport_->close();
    }
}
