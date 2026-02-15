#ifndef PANCAKE_PROXY_CLIENT_H
#define PANCAKE_PROXY_CLIENT_H
#include <memory>
#include <string>
#include <vector>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include "client.h"
#include "../../thrift/pancake.thrift"

class proxy_client final : public client {
public:
    proxy_client() = default;
    ~proxy_client() override = default;
    proxy_client(const proxy_client&) = delete;
    proxy_client& operator=(const proxy_client&) = delete;
    void init(const std::string& host, int port) override;
    std::string get(const std::string& key) override;
    void put(const std::string& key, const std::string& value) override;
    std::vector<std::string> get_batch(const std::vector<std::string>& keys) override;
    void put_batch(const std::vector<std::string>& keys, const std::vector<std::string>& values) override;

private:
    std::shared_ptr<apache::thrift::transport::TTransport> transport_;
    std::shared_ptr<pancake::pancake_thriftClient> client_;
};
#endif //PANCAKE_PROXY_CLIENT_H