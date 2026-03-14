#ifndef THRIFT_RESPONSE_SERVICE_H
#define THRIFT_RESPONSE_SERVICE_H

#pragma once
#include <thrift/TDispatchProcessor.h>
#include <thrift/TApplicationException.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include <thrift/protocol/TProtocol.h>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "proxy_types.h"

class pancake_thrift_responseIf {
public:
    virtual ~pancake_thrift_responseIf() {}
    virtual void async_response(const sequence_id& seq_id, const int32_t op_code, const std::vector<std::string>& result) = 0;
};

class pancake_thrift_responseIfFactory {
public:
    typedef pancake_thrift_responseIf Handler;
    virtual ~pancake_thrift_responseIfFactory() {}
    virtual pancake_thrift_responseIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
    virtual void releaseHandler(pancake_thrift_responseIf* handler) = 0;
};

class pancake_thrift_responseIfSingletonFactory : virtual public pancake_thrift_responseIfFactory {
public:
    explicit pancake_thrift_responseIfSingletonFactory(std::shared_ptr<pancake_thrift_responseIf> iface)
        : iface_(iface) {}
    pancake_thrift_responseIf* getHandler(const ::apache::thrift::TConnectionInfo&) override { return iface_.get(); }
    void releaseHandler(pancake_thrift_responseIf*) override {}
private:
    std::shared_ptr<pancake_thrift_responseIf> iface_;
};

struct _pancake_thrift_response_async_response_args__isset {
    _pancake_thrift_response_async_response_args__isset() : seq_id(false), op_code(false), result(false) {}
    bool seq_id : 1;
    bool op_code : 1;
    bool result : 1;
};

class pancake_thrift_response_async_response_args {
public:
    pancake_thrift_response_async_response_args() : op_code(0) {}
    virtual ~pancake_thrift_response_async_response_args() throw() {}
    sequence_id seq_id;
    int32_t op_code;
    std::vector<std::string> result;
    _pancake_thrift_response_async_response_args__isset __isset;
    uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
    uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_response_async_response_pargs {
public:
    virtual ~pancake_thrift_response_async_response_pargs() throw() {}
    const sequence_id* seq_id{nullptr};
    const int32_t* op_code{nullptr};
    const std::vector<std::string>* result{nullptr};
    uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_responseClient : virtual public pancake_thrift_responseIf {
public:
    explicit pancake_thrift_responseClient(std::shared_ptr<::apache::thrift::protocol::TProtocol> prot) {
        setProtocol(prot, prot);
    }

    pancake_thrift_responseClient(std::shared_ptr<::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr<::apache::thrift::protocol::TProtocol> oprot) {
        setProtocol(iprot, oprot);
    }

    void async_response(const sequence_id& seq_id, const int32_t op_code, const std::vector<std::string>& result) override;
    void send_async_response(const sequence_id& seq_id, const int32_t op_code, const std::vector<std::string>& result);

private:
    void setProtocol(std::shared_ptr<::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr<::apache::thrift::protocol::TProtocol> oprot) {
        piprot_ = iprot;
        poprot_ = oprot;
        iprot_ = iprot.get();
        oprot_ = oprot.get();
    }

    std::shared_ptr<::apache::thrift::protocol::TProtocol> piprot_;
    std::shared_ptr<::apache::thrift::protocol::TProtocol> poprot_;
    ::apache::thrift::protocol::TProtocol* iprot_{nullptr};
    ::apache::thrift::protocol::TProtocol* oprot_{nullptr};
};

class pancake_thrift_responseProcessor : public ::apache::thrift::TDispatchProcessor {
protected:
    std::shared_ptr<pancake_thrift_responseIf> iface_;
    bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext) override;

private:
    typedef void (pancake_thrift_responseProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
    typedef std::map<std::string, ProcessFunction> ProcessMap;
    ProcessMap processMap_;
    void process_async_response(int32_t, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);

public:
    explicit pancake_thrift_responseProcessor(std::shared_ptr<pancake_thrift_responseIf> iface)
        : iface_(iface) {
        processMap_["async_response"] = &pancake_thrift_responseProcessor::process_async_response;
    }
};

class pancake_thrift_responseProcessorFactory : public ::apache::thrift::TProcessorFactory {
public:
    explicit pancake_thrift_responseProcessorFactory(
        std::shared_ptr<pancake_thrift_responseIfFactory> handlerFactory)
        : handlerFactory_(handlerFactory) {}

    std::shared_ptr<::apache::thrift::TProcessor>
    getProcessor(const ::apache::thrift::TConnectionInfo& connInfo) override;

private:
    std::shared_ptr<pancake_thrift_responseIfFactory> handlerFactory_;
};

#endif
