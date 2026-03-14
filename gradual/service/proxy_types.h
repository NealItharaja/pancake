#ifndef PROXY_TYPES_H
#define PROXY_TYPES_H
#pragma once
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>
#include <cstdint>
#include <string>
typedef struct _sequence_id__isset {
    _sequence_id__isset() : client_id(false), seq(false) {}
    bool client_id : 1;
    bool seq : 1;
} _sequence_id__isset;
class sequence_id {
public:
    sequence_id() : client_id(0), seq(0) {}
    int64_t client_id;
    int64_t seq;
    _sequence_id__isset __isset;
    void __set_client_id(const int64_t v) { client_id = v; __isset.client_id = true; }
    void __set_seq(const int64_t v) { seq = v; __isset.seq = true; }

    bool operator==(const sequence_id& rhs) const {
        return client_id == rhs.client_id && seq == rhs.seq;
    }
    bool operator<(const sequence_id& rhs) const {
        if (client_id != rhs.client_id) return client_id < rhs.client_id;
        return seq < rhs.seq;
    }
    uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
    uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};
#endif
