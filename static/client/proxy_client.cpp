#include "proxy_client.h"

using apache::thrift::protocol::TBinaryProtocol;
using apache::thrift::transport::TSocket;
using apache::thrift::transport::TFramedTransport;

void proxy_client::init(const std::string& host, int port) {
    auto socket = std::make_shared<TSocket>(host, port);
    transport_ = std::make_shared<TFramedTransport>(socket);
    auto protocol = std::make_shared<TBinaryProtocol>(transport_);
    client_ = std::make_shared<pancake::pancake_thriftClient>(protocol);
    transport_->open();
}

std::string proxy_client::get(const std::string& key) {
    std::string result;
    client_->get(result, key);
    return result;
}

void proxy_client::put(const std::string& key, const std::string& value) {
    client_->put(key, value);
}

std::vector<std::string> proxy_client::get_batch(const std::vector<std::string>& keys) {
    std::vector<std::string> result;
    client_->get_batch(result, keys);
    return result;
}

void proxy_client::put_batch(const std::vector<std::string>& keys, const std::vector<std::string>& values) {
    client_->put_batch(keys, values);
}
