#ifndef THRIFT_HELPERS_H
#define THRIFT_HELPERS_H

#pragma once

#include <memory>
#include <string>
#include <stdexcept>

// Thrift transport / protocol
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

// Thrift servers (NO concurrency headers in Thrift >= 0.18)
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/server/TThreadPoolServer.h>

namespace thrift_helpers {

    // ------------------------------------------------------------
    // Client helpers
    // ------------------------------------------------------------
    inline void make_framed_binary_client(
        const std::string& host,
        int port,
        std::shared_ptr<apache::thrift::protocol::TProtocol>& out_proto,
        std::shared_ptr<apache::thrift::transport::TTransport>& out_transport
    ) {
        using namespace apache::thrift;
        using namespace apache::thrift::transport;
        using namespace apache::thrift::protocol;

        auto socket = std::make_shared<TSocket>(host, port);
        auto transport = std::make_shared<TFramedTransport>(socket);
        auto protocol = std::make_shared<TBinaryProtocol>(transport);

        transport->open();

        out_proto = protocol;
        out_transport = transport;
    }

    // ------------------------------------------------------------
    // Server helpers
    // ------------------------------------------------------------
    inline std::shared_ptr<apache::thrift::transport::TServerTransport>
    make_server_socket(int port) {
        using namespace apache::thrift::transport;
        return std::make_shared<TServerSocket>(port);
    }

    inline std::shared_ptr<apache::thrift::transport::TTransportFactory>
    make_framed_factory() {
        using namespace apache::thrift::transport;
        return std::make_shared<TFramedTransportFacto_
