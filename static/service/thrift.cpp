#include "thrift.h"

using ::apache::thrift::protocol::TType;

static uint32_t read_string_list(::apache::thrift::protocol::TProtocol* iprot, std::vector<std::string>& out) {
  uint32_t xfer = 0;
  out.clear();
  ::apache::thrift::protocol::TType et;
  uint32_t n = 0;
  xfer += iprot->readListBegin(et, n);
  out.resize(n);
  for (uint32_t i = 0; i < n; i++) xfer += iprot->readString(out[i]);
  xfer += iprot->readListEnd();
  return xfer;
}

static uint32_t write_string_list(::apache::thrift::protocol::TProtocol* oprot, const std::vector<std::string>& v) {
  uint32_t xfer = 0;
  xfer += oprot->writeListBegin(::apache::thrift::protocol::T_STRING, (uint32_t)v.size());

  for (const auto& s : v) {
    xfer += oprot->writeString(s);
  }

  xfer += oprot->writeListEnd();
  return xfer;
}

uint32_t pancake_thrift_get_client_id_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) { xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) break;
    xfer += iprot->skip(ftype);
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_client_id_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_get_client_id_args");
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_register_client_id_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) break;
    if (fid == 1 && ftype == ::apache::thrift::protocol::T_I32) xfer += iprot->readI32(block_id);
    else if (fid == 2 && ftype == ::apache::thrift::protocol::T_I64) xfer += iprot->readI64(client_id);
    else xfer += iprot->skip(ftype);
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_register_client_id_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_register_client_id_args");
  xfer += oprot->writeFieldBegin("block_id", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(block_id);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("client_id", ::apache::thrift::protocol::T_I64, 2);
  xfer += oprot->writeI64(client_id);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_async_get_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);
  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);

    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }

    if (fid == 1 && ftype == ::apache::thrift::protocol::T_STRUCT) {
      xfer += seq_id.read(iprot);
    } else if (fid == 2 && ftype == ::apache::thrift::protocol::T_STRING) {
      xfer += iprot->readString(key);
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}
uint32_t pancake_thrift_async_get_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_async_get_args");
  xfer += oprot->writeFieldBegin("seq_id", ::apache::thrift::protocol::T_STRUCT, 1);
  xfer += seq_id.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("key", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(key);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_async_put_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    if (fid == 1 && ftype == ::apache::thrift::protocol::T_STRUCT) {
      xfer += seq_id.read(iprot);
    } else if (fid == 2 && ftype == ::apache::thrift::protocol::T_STRING) {
      xfer += iprot->readString(key);
    } else if (fid == 3 && ftype == ::apache::thrift::protocol::T_STRING) {
      xfer += iprot->readString(value);
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_async_put_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_async_put_args");
  xfer += oprot->writeFieldBegin("seq_id", ::apache::thrift::protocol::T_STRUCT, 1);
  xfer += seq_id.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("key", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(key);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("value", ::apache::thrift::protocol::T_STRING, 3);
  xfer += oprot->writeString(value);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_async_get_batch_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) break;
    if (fid == 1 && ftype == ::apache::thrift::protocol::T_STRUCT) xfer += seq_id.read(iprot);
    else if (fid == 2 && ftype == ::apache::thrift::protocol::T_LIST) xfer += read_string_list(iprot, keys);
    else xfer += iprot->skip(ftype);
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}
uint32_t pancake_thrift_async_get_batch_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_async_get_batch_args");
  xfer += oprot->writeFieldBegin("seq_id", ::apache::thrift::protocol::T_STRUCT, 1);
  xfer += seq_id.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("keys", ::apache::thrift::protocol::T_LIST, 2);
  xfer += write_string_list(oprot, keys);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_async_put_batch_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    if (fid == 1 && ftype == ::apache::thrift::protocol::T_STRUCT) {
      xfer += seq_id.read(iprot);
    } else if (fid == 2 && ftype == ::apache::thrift::protocol::T_LIST) {
      xfer += read_string_list(iprot, keys);
    } else if (fid == 3 && ftype == ::apache::thrift::protocol::T_LIST) {
      xfer += read_string_list(iprot, values);
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_async_put_batch_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_async_put_batch_args");
  xfer += oprot->writeFieldBegin("seq_id", ::apache::thrift::protocol::T_STRUCT, 1);
  xfer += seq_id.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("keys", ::apache::thrift::protocol::T_LIST, 2);
  xfer += write_string_list(oprot, keys);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("values", ::apache::thrift::protocol::T_LIST, 3);
  xfer += write_string_list(oprot, values);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    if (fid == 1 && ftype == ::apache::thrift::protocol::T_STRING) {
      xfer += iprot->readString(key);
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_get_args");
  xfer += oprot->writeFieldBegin("key", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(key);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_put_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    if (fid == 1 && ftype == ::apache::thrift::protocol::T_STRING) {
      xfer += iprot->readString(key);
    } else if (fid == 2 && ftype == ::apache::thrift::protocol::T_STRING) {
      xfer += iprot->readString(value);
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_put_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_put_args");
  xfer += oprot->writeFieldBegin("key", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(key);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("value", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(value);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_batch_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    if (fid == 1 && ftype == ::apache::thrift::protocol::T_LIST) {
      xfer += read_string_list(iprot, keys);
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_batch_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_get_batch_args");
  xfer += oprot->writeFieldBegin("keys", ::apache::thrift::protocol::T_LIST, 1);
  xfer += write_string_list(oprot, keys);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_put_batch_args::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    if (fid == 1 && ftype == ::apache::thrift::protocol::T_LIST) {
      xfer += read_string_list(iprot, keys);
    } else if (fid == 2 && ftype == ::apache::thrift::protocol::T_LIST) {
      xfer += read_string_list(iprot, values);
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_put_batch_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_put_batch_args");
  xfer += oprot->writeFieldBegin("keys", ::apache::thrift::protocol::T_LIST, 1);
  xfer += write_string_list(oprot, keys);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("values", ::apache::thrift::protocol::T_LIST, 2);
  xfer += write_string_list(oprot, values);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_client_id_result::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    if (fid == 0 && ftype == ::apache::thrift::protocol::T_I64) { 
      xfer += iprot->readI64(success); __isset_success = true; 
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}
uint32_t pancake_thrift_get_client_id_result::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_get_client_id_result");

  if (__isset_success) {
    xfer += oprot->writeFieldBegin("success", ::apache::thrift::protocol::T_I64, 0);
    xfer += oprot->writeI64(success);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_result::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname;
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    if (fid == 0 && ftype == ::apache::thrift::protocol::T_STRING) { 
      xfer += iprot->readString(success); __isset_success = true; 
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_result::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_get_result");

  if (__isset_success) {
    xfer += oprot->writeFieldBegin("success", ::apache::thrift::protocol::T_STRING, 0);
    xfer += oprot->writeString(success);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_batch_result::read(::apache::thrift::protocol::TProtocol* iprot) {
  uint32_t xfer = 0; 
  std::string fname; 
  TType ftype; 
  int16_t fid;
  xfer += iprot->readStructBegin(fname);

  while (true) {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    if (fid == 0 && ftype == ::apache::thrift::protocol::T_LIST) { 
      xfer += read_string_list(iprot, success); __isset_success = true; 
    } else {
      xfer += iprot->skip(ftype);
    }
    xfer += iprot->readFieldEnd();
  }
  xfer += iprot->readStructEnd();
  return xfer;
}

uint32_t pancake_thrift_get_batch_result::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("pancake_thrift_get_batch_result");

  if (__isset_success) {
    xfer += oprot->writeFieldBegin("success", ::apache::thrift::protocol::T_LIST, 0);
    xfer += write_string_list(oprot, success);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

pancake_thriftProcessor::pancake_thriftProcessor(std::shared_ptr<pancake_thriftIf> iface)
  : iface_(iface) {
  processMap_["get_client_id"] = &pancake_thriftProcessor::process_get_client_id;
  processMap_["register_client_id"] = &pancake_thriftProcessor::process_register_client_id;
  processMap_["async_get"] = &pancake_thriftProcessor::process_async_get;
  processMap_["async_put"] = &pancake_thriftProcessor::process_async_put;
  processMap_["async_get_batch"] = &pancake_thriftProcessor::process_async_get_batch;
  processMap_["async_put_batch"] = &pancake_thriftProcessor::process_async_put_batch;
  processMap_["get"] = &pancake_thriftProcessor::process_get;
  processMap_["put"] = &pancake_thriftProcessor::process_put;
  processMap_["get_batch"] = &pancake_thriftProcessor::process_get_batch;
  processMap_["put_batch"] = &pancake_thriftProcessor::process_put_batch;
}

bool pancake_thriftProcessor::dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext) {
  auto it = processMap_.find(fname);

  if (it == processMap_.end()) {
    iprot->skip(::apache::thrift::protocol::T_STRUCT);
    iprot->readMessageEnd();
    iprot->getTransport()->readEnd();
    ::apache::thrift::TApplicationException x(::apache::thrift::TApplicationException::UNKNOWN_METHOD, "Invalid method name: '" + fname + "'");
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

void pancake_thriftProcessor::process_get_client_id(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void*) {
  pancake_thrift_get_client_id_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  pancake_thrift_get_client_id_result result;
  result.success = iface_->get_client_id();
  result.__isset_success = true;
  oprot->writeMessageBegin("get_client_id", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  oprot->getTransport()->writeEnd();
  oprot->getTransport()->flush();
}

void pancake_thriftProcessor::process_register_client_id(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void*) {
  pancake_thrift_register_client_id_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  iface_->register_client_id(args.block_id, args.client_id);
  oprot->writeMessageBegin("register_client_id", ::apache::thrift::protocol::T_REPLY, seqid);
  oprot->writeStructBegin("pancake_thrift_register_client_id_result");
  oprot->writeFieldStop();
  oprot->writeStructEnd();
  oprot->writeMessageEnd();
  oprot->getTransport()->writeEnd();
  oprot->getTransport()->flush();
}

void pancake_thriftProcessor::process_async_get(int32_t, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol*, void*) {
  pancake_thrift_async_get_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  iface_->async_get(args.seq_id, args.key);
}

void pancake_thriftProcessor::process_async_put(int32_t, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol*, void*) {
  pancake_thrift_async_put_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  iface_->async_put(args.seq_id, args.key, args.value);
}

void pancake_thriftProcessor::process_async_get_batch(int32_t, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol*, void*) {
  pancake_thrift_async_get_batch_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  iface_->async_get_batch(args.seq_id, args.keys);
}

void pancake_thriftProcessor::process_async_put_batch(int32_t, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol*, void*) {
  pancake_thrift_async_put_batch_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  iface_->async_put_batch(args.seq_id, args.keys, args.values);
}

void pancake_thriftProcessor::process_get(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void*) {
  pancake_thrift_get_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  pancake_thrift_get_result result;
  iface_->get(result.success, args.key);
  result.__isset_success = true;
  oprot->writeMessageBegin("get", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  oprot->getTransport()->writeEnd();
  oprot->getTransport()->flush();
}

void pancake_thriftProcessor::process_put(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void*) {
  pancake_thrift_put_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  iface_->put(args.key, args.value);
  oprot->writeMessageBegin("put", ::apache::thrift::protocol::T_REPLY, seqid);
  oprot->writeStructBegin("pancake_thrift_put_result");
  oprot->writeFieldStop();
  oprot->writeStructEnd();
  oprot->writeMessageEnd();
  oprot->getTransport()->writeEnd();
  oprot->getTransport()->flush();
}

void pancake_thriftProcessor::process_get_batch(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void*) {
  pancake_thrift_get_batch_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  pancake_thrift_get_batch_result result;
  iface_->get_batch(result.success, args.keys);
  result.__isset_success = true;
  oprot->writeMessageBegin("get_batch", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  oprot->getTransport()->writeEnd();
  oprot->getTransport()->flush();
}

void pancake_thriftProcessor::process_put_batch(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void*) {
  pancake_thrift_put_batch_args args; args.read(iprot);
  iprot->readMessageEnd(); iprot->getTransport()->readEnd();
  iface_->put_batch(args.keys, args.values);
  oprot->writeMessageBegin("put_batch", ::apache::thrift::protocol::T_REPLY, seqid);
  oprot->writeStructBegin("pancake_thrift_put_batch_result");
  oprot->writeFieldStop();
  oprot->writeStructEnd();
  oprot->writeMessageEnd();
  oprot->getTransport()->writeEnd();
  oprot->getTransport()->flush();
}
