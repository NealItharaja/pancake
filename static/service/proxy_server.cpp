#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thrift/server/TThreadedServer.h>
#include "kv_store.h"
#include "thrift_helpers.h"
#include "thrift.h"
#include "thrift_response_service.h"

using apache::thrift::server::TThreadedServer;

class PancakeHandler final : public pancake_thriftIf {
public:
  PancakeHandler(std::string resp_host, int resp_port)
    : resp_host_(std::move(resp_host)), resp_port_(resp_port) {}

  int64_t get_client_id() override {
    static int64_t cid = 1000;
    return cid++;
  }

  void register_client_id(const int32_t , const int64_t) override {
  }

  void get(std::string& _return, const std::string& key) override {
    auto v = kv_.get(key);
    _return = v ? *v : std::string{};
  }

  void put(const std::string& key, const std::string& value) override {
    kv_.put(key, value);
  }

  void get_batch(std::vector<std::string>& _return,
                 const std::vector<std::string>& keys) override {
    _return = kv_.get_batch(keys);
  }

  void put_batch(const std::vector<std::string>& keys,
                 const std::vector<std::string>& values) override {
    kv_.put_batch(keys, values);
  }
  void async_get(const sequence_id& seq_id, const std::string& key) override {
    std::vector<std::string> result;
    auto v = kv_.get(key);
    result.push_back(v ? *v : std::string{});
    send_async_response(seq_id, 1, result);
  }

  void async_put(const sequence_id& seq_id, const std::string& key, const std::string& value) override {
    kv_.put(key, value);
    std::vector<std::string> result;
    result.push_back("OK");
    send_async_response(seq_id, 2, result);
  }

  void async_get_batch(const sequence_id& seq_id, const std::vector<std::string>& keys) override {
    auto result = kv_.get_batch(keys);
    send_async_response(seq_id, 3, result);
  }

  void async_put_batch(const sequence_id& seq_id,
                       const std::vector<std::string>& keys,
                       const std::vector<std::string>& values) override {
    kv_.put_batch(keys, values);
    std::vector<std::string> result;
    result.push_back("OK");
    send_async_response(seq_id,4, result);
  }

private:
  void send_async_response(const sequence_id& seq_id,
                           const int32_t op_code,
                           const std::vector<std::string>& result) {
    try {
      std::shared_ptr<apache::thrift::protocol::TProtocol> proto;
      std::shared_ptr<apache::thrift::transport::TTransport> transport;

      thrift_helpers::make_framed_binary_client(resp_host_, resp_port_, proto, transport);

      pancake_thrift_responseClient resp_client(proto);
      resp_client.async_response(seq_id, op_code, result);

      transport->close();
    } catch (const std::exception& e) {
      std::cerr << "[WARN] async_response failed: " << e.what() << "\n";
    }
  }
private:
  KVStore kv_;
  std::string resp_host_;
  int resp_port_;
};
static void usage(const char* argv0) {
  std::cerr
    << "Usage:\n"
    << "  " << argv0 << " <listen_port> <response_host> <response_port>\n\n"
    << "Example:\n"
    << "  " << argv0 << " 9090 127.0.0.1 9091\n";
}
int main(int argc, char** argv) {
  if (argc != 4) {
    usage(argv[0]);
    return 1;
  }
  const int listen_port = std::stoi(argv[1]);
  const std::string resp_host = argv[2];
  const int resp_port = std::stoi(argv[3]);
  auto handler = std::make_shared<PancakeHandler>(resp_host, resp_port);
  auto processor = std::make_shared<pancake_thriftProcessor>(handler);
  auto serverTransport = thrift_helpers::make_server_socket(listen_port);
  auto transportFactory = thrift_helpers::make_framed_factory();
  auto protocolFactory  = thrift_helpers::make_binary_factory();
  TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
  std::cout << "Proxy server listening on port " << listen_port
            << " (response service at " << resp_host << ":" << resp_port << ")\n";
  server.serve();
  return 0;
}
