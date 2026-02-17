#include "thrift_response_service.h"
#include <thrift/transport/TTransport.h>
#include <thrift/protocol/TProtocol.h>

uint32_t pancake_thrift_response_async_response_args::read(::apache::thrift::protocol::TProtocol* iprot) {
    uint32_t xfer = 0;
    std::string fname;
    ::apache::thrift::protocol::TType ftype;
    int16_t fid;
    xfer += iprot->readStructBegin(fname);

    while (true) {
        xfer += iprot->readFieldBegin(fname, ftype, fid);
        if (ftype == ::apache::thrift::protocol::T_STOP) break;
        switch (fid) {
            case 1:
                if (ftype == ::apache::thrift::protocol::T_STRUCT) { xfer += seq_id.read(iprot); __isset.seq_id = true; }
                else xfer += iprot->skip(ftype);
                break;
            case 2:
                if (ftype == ::apache::thrift::protocol::T_I32) { xfer += iprot->readI32(op_code); __isset.op_code = true; }
                else xfer += iprot->skip(ftype);
                break;
            case 3:
                if (ftype == ::apache::thrift::protocol::T_LIST) {
                    result.clear();
                    uint32_t n; ::apache::thrift::protocol::TType et;
                    xfer += iprot->readListBegin(et, n);
                    result.resize(n);
                    for (uint32_t i = 0; i < n; i++) xfer += iprot->readString(result[i]);
                    xfer += iprot->readListEnd();
                    __isset.result = true;
                } else xfer += iprot->skip(ftype);
                break;
            default: xfer += iprot->skip(ftype); break;
        }
        xfer += iprot->readFieldEnd();
    }
    xfer += iprot->readStructEnd();
    return xfer;
}

uint32_t pancake_thrift_response_async_response_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
    uint32_t xfer = 0;
    xfer += oprot->writeStructBegin("pancake_thrift_response_async_response_args");
    xfer += oprot->writeFieldBegin("seq_id", ::apache::thrift::protocol::T_STRUCT, 1);
    xfer += seq_id.write(oprot);
    xfer += oprot->writeFieldEnd();
    xfer += oprot->writeFieldBegin("op_code", ::apache::thrift::protocol::T_I32, 2);
    xfer += oprot->writeI32(op_code);
    xfer += oprot->writeFieldEnd();
    xfer += oprot->writeFieldBegin("result", ::apache::thrift::protocol::T_LIST, 3);
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_STRING, (uint32_t)result.size());

    for (const auto& s : result) {
        xfer += oprot->writeString(s);
    }

    xfer += oprot->writeListEnd();
    xfer += oprot->writeFieldEnd();
    xfer += oprot->writeFieldStop();
    xfer += oprot->writeStructEnd();
    return xfer;
}

uint32_t pancake_thrift_response_async_response_pargs::write(::apache::thrift::protocol::TProtocol* oprot) const {
    uint32_t xfer = 0;
    xfer += oprot->writeStructBegin("pancake_thrift_response_async_response_pargs");
    xfer += oprot->writeFieldBegin("seq_id", ::apache::thrift::protocol::T_STRUCT, 1);
    xfer += seq_id->write(oprot);
    xfer += oprot->writeFieldEnd();
    xfer += oprot->writeFieldBegin("op_code", ::apache::thrift::protocol::T_I32, 2);
    xfer += oprot->writeI32(*op_code);
    xfer += oprot->writeFieldEnd();
    xfer += oprot->writeFieldBegin("result", ::apache::thrift::protocol::T_LIST, 3);
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_STRING, (uint32_t)result->size());
    for (const auto& s : *result) {
        xfer += oprot->writeString(s);
    }

    xfer += oprot->writeListEnd();
    xfer += oprot->writeFieldEnd();
    xfer += oprot->writeFieldStop();
    xfer += oprot->writeStructEnd();
    return xfer;
}

void pancake_thrift_responseClient::async_response(const sequence_id& seq_id, const int32_t op_code, const std::vector<std::string>& result) {
    send_async_response(seq_id, op_code, result);
}

void pancake_thrift_responseClient::send_async_response(const sequence_id& seq_id, const int32_t op_code, const std::vector<std::string>& result) {
    int32_t cseqid = 0;
    oprot_->writeMessageBegin("async_response", ::apache::thrift::protocol::T_ONEWAY, cseqid);
    pancake_thrift_response_async_response_pargs args;
    args.seq_id = &seq_id;
    args.op_code = &op_code;
    args.result = &result;
    args.write(oprot_);
    oprot_->writeMessageEnd();
    oprot_->getTransport()->writeEnd();
    oprot_->getTransport()->flush();
}

bool pancake_thrift_responseProcessor::dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext) {
    auto it = processMap_.find(fname);

    if (it == processMap_.end()) {
        iprot->skip(::apache::thrift::protocol::T_STRUCT);
        iprot->readMessageEnd();
        iprot->getTransport()->readEnd();

        ::apache::thrift::TApplicationException x(
            ::apache::thrift::TApplicationException::UNKNOWN_METHOD,
            "Invalid method name: '" + fname + "'");

        oprot->writeMessageBegin(fname, ::apache::thrift::protocol::T_EXCEPTION, seqid);
        x.write(oprot);
        oprot->writeMessageEnd();
        oprot->getTransport()->writeEnd();
        oprot->getTransport()->flush();
        return true;
    }
    (this->*(it->second))(seqid, iprot, oprot, callContext);
    return true;
}

void pancake_thrift_responseProcessor::process_async_response(
    int32_t,
    ::apache::thrift::protocol::TProtocol* iprot,
    ::apache::thrift::protocol::TProtocol*,
    void* callContext) {
    pancake_thrift_response_async_response_args args;
    args.read(iprot);
    iprot->readMessageEnd();
    iprot->getTransport()->readEnd();
    iface_->async_response(args.seq_id, args.op_code, args.result);
}

// Processor factory
std::shared_ptr<::apache::thrift::TProcessor>
pancake_thrift_responseProcessorFactory::getProcessor(const ::apache::thrift::TConnectionInfo& connInfo) {
    pancake_thrift_responseIf* handler = handlerFactory_->getHandler(connInfo);
    return std::make_shared<pancake_thrift_responseProcessor>(std::shared_ptr<pancake_thrift_responseIf>(handler));
}
