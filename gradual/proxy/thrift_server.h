#ifndef THRIFT_SERVER_H
#define THRIFT_SERVER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <thrift/server/TThreadedServer.h>

class proxy;
using thrift_response_client_map = std::unordered_map<int64_t, int64_t>;

namespace thrift_server {
    std::shared_ptr<apache::thrift::server::TThreadedServer> create(std::shared_ptr<proxy> proxy_instance, const std::string& service_name, std::shared_ptr<thrift_response_client_map> response_map, int port, int threads);
}

#endif // THRIFT_SERVER_H
