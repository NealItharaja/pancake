#ifndef PANCAKE_COMMAND_READER_H
#define PANCAKE_COMMAND_READER_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TTransport.h>
#include <memory>
#include <vector>
#include <string>

class command_reader {
public:
    command_reader() = default;
    explicit command_reader(std::shared_ptr<apache::thrift::protocol::TProtocol> protocol);
    int64_t recv_response(std::vector<std::string>& out);

private:
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol_;
    apache::thrift::protocol::TProtocol* iprot_{nullptr};
};
#endif //PANCAKE_COMMAND_READER_H