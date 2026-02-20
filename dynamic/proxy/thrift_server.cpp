#include "thrift_server.h"
#include "../proxy.h"
#include "pancakeProxy/pancake_proxy.h"
#include "../service/thrift.h"
#include "../service/thrift_helpers.h"
#include <thrift/server/TThreadedServer.h>

class ProxyThriftHandler : public pancake_thriftIf {
public:
    explicit ProxyThriftHandler(std::shared_ptr<proxy> proxy_instance)
        : proxy_instance_(std::dynamic_pointer_cast<pancake_proxy>(proxy_instance)) {}

    int64_t get_client_id() override {
        static int64_t cid = 1000;
        return cid++;
    }

    void register_client_id(const int32_t, const int64_t) override { }

    void get(std::string& _return, const std::string& key) override {
        if (proxy_instance_) {
            _return = proxy_instance_->get(key);
        }
    }

    void put(const std::string& key, const std::string& value) override {
        if (proxy_instance_) {
            proxy_instance_->put(key, value);
        }
    }

    void get_batch(std::vector<std::string>& _return, const std::vector<std::string>& keys) override {
        if (proxy_instance_) {
            _return = proxy_instance_->get_batch(keys);
        }
    }

    void put_batch(const std::vector<std::string>& keys, const std::vector<std::string>& values) override {
        if (proxy_instance_) {
            proxy_instance_->put_batch(keys, values);
        }
    }

    void async_get(const sequence_id&, const std::string&) override { }

    void async_put(const sequence_id&, const std::string&, const std::string&) override { }

    void async_get_batch(const sequence_id&, const std::vector<std::string>&) override { }

    void async_put_batch(const sequence_id&, const std::vector<std::string>&, const std::vector<std::string>&) override { }

private:
    std::shared_ptr<pancake_proxy> proxy_instance_;
};

std::shared_ptr<apache::thrift::server::TThreadedServer> thrift_server::create(std::shared_ptr<proxy> proxy_instance, const std::string&, std::shared_ptr<thrift_response_client_map>, int port, int) {
    auto handler = std::make_shared<ProxyThriftHandler>(proxy_instance);
    auto processor = std::make_shared<pancake_thriftProcessor>(handler);
    auto serverTransport = thrift_helpers::make_server_socket(port);
    auto transportFactory = thrift_helpers::make_framed_factory();
    auto protocolFactory = thrift_helpers::make_binary_factory();
    return std::make_shared<apache::thrift::server::TThreadedServer>(processor, serverTransport, transportFactory, protocolFactory);
}
