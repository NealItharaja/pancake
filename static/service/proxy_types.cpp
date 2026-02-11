#include "proxy_types.h"
uint32_t sequence_id::read(::apache::thrift::protocol::TProtocol* iprot) {
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
                if (ftype == ::apache::thrift::protocol::T_I64) {
                    xfer += iprot->readI64(client_id);
                    __isset.client_id = true;
                } else xfer += iprot->skip(ftype);
            break;
            case 2:
                if (ftype == ::apache::thrift::protocol::T_I64) {
                    xfer += iprot->readI64(seq);
                    __isset.seq = true;
                } else xfer += iprot->skip(ftype);
            break;
            default:
                xfer += iprot->skip(ftype);
            break;
        }
        xfer += iprot->readFieldEnd();
    }
    xfer += iprot->readStructEnd();
    return xfer;
}
uint32_t sequence_id::write(::apache::thrift::protocol::TProtocol* oprot) const {
    uint32_t xfer = 0;
    xfer += oprot->writeStructBegin("sequence_id");
    xfer += oprot->writeFieldBegin("client_id", ::apache::thrift::protocol::T_I64, 1);
    xfer += oprot->writeI64(client_id);
    xfer += oprot->writeFieldEnd();
    xfer += oprot->writeFieldBegin("seq", ::apache::thrift::protocol::T_I64, 2);
    xfer += oprot->writeI64(seq);
    xfer += oprot->writeFieldEnd();
    xfer += oprot->writeFieldStop();
    xfer += oprot->writeStructEnd();
    return xfer;
}
