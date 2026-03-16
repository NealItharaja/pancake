#include "thrift_server.h"
#include "../proxy.h"
#include "pancakeProxy/pancake_proxy.h"
#include "../service/thrift.h"
#include "../service/thrift_helpers.h"
#include <algorithm>
#include <iostream>
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
        try {
            if (proxy_instance_) {
                _return = proxy_instance_->get(key);
            }
        } catch (const std::exception& e) {
            std::cerr << "[thrift][get] " << e.what() << "\n";
            _return.clear();
        }
    }

    void put(const std::string& key, const std::string& value) override {
        try {
            if (proxy_instance_) {
                proxy_instance_->put(key, value);
            }
        } catch (const std::exception& e) {
            std::cerr << "[thrift][put] " << e.what() << "\n";
        }
    }

    void get_batch(std::vector<std::string>& _return, const std::vector<std::string>& keys) override {
        try {
            if (proxy_instance_) {
                _return = proxy_instance_->get_batch(keys);
            }
        } catch (const std::exception& e) {
            std::cerr << "[thrift][get_batch] " << e.what() << "\n";
            _return.clear();
        }
    }

    void put_batch(const std::vector<std::string>& keys, const std::vector<std::string>& values) override {
        try {
            if (proxy_instance_) {
                proxy_instance_->put_batch(keys, values);
            }
        } catch (const std::exception& e) {
            std::cerr << "[thrift][put_batch] " << e.what() << "\n";
        }
    }

    void async_get(const sequence_id&, const std::string&) override { }

    void async_put(const sequence_id&, const std::string&, const std::string&) override { }

    void async_get_batch(const sequence_id&, const std::vector<std::string>&) override { }

    void async_put_batch(const sequence_id&, const std::vector<std::string>&, const std::vector<std::string>&) override { }

private:
    std::shared_ptr<pancake_proxy> proxy_instance_;
};

std::shared_ptr<apache::thrift::server::TThreadedServer> thrift_server::create(std::shared_ptr<proxy> proxy_instance, const std::string&, std::shared_ptr<thrift_response_client_map>, int port, int threads) {
    auto handler = std::make_shared<ProxyThriftHandler>(proxy_instance);
    auto processor = std::make_shared<pancake_thriftProcessor>(handler);
    auto serverTransport = thrift_helpers::make_server_socket(port);
    auto transportFactory = thrift_helpers::make_framed_factory();
    auto protocolFactory = thrift_helpers::make_binary_factory();
    auto server = std::make_shared<apache::thrift::server::TThreadedServer>(processor, serverTransport, transportFactory, protocolFactory);
    server->setConcurrentClientLimit(std::max(1, threads));
    return server;
}
