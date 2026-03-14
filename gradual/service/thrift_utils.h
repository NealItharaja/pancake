#ifndef PANCAKE_THRIFT_UTILS_H
#define PANCAKE_THRIFT_UTILS_H

#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportException.h>
#include <memory>
#include <string>
#ifdef _WIN32
#include <windows.h>
#define usleep(x) Sleep((x) / 1000)
#else
#include <unistd.h>
#endif

inline void wait_for_server_start(const std::string &host, int port) {
    using apache::thrift::transport::TFramedTransport;
    using apache::thrift::transport::TSocket;
    using apache::thrift::transport::TTransport;
    using apache::thrift::transport::TTransportException;
    bool waiting = true;

    while (waiting) {
        try {
            std::shared_ptr<TTransport> transport = std::make_shared<TFramedTransport>(std::make_shared<TSocket>(host, port));
            transport->open();
            transport->close();
            waiting = false;
        }
        catch (const TTransportException &) {
            usleep(100000);
        }
    }
}

#endif // PANCAKE_THRIFT_UTILS_H
