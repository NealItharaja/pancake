#ifndef THRIFT_H
#define THRIFT_H

#pragma once
#include <thrift/TDispatchProcessor.h>
#include <thrift/TApplicationException.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include "proxy_types.h"

class pancake_thriftIf {
public:
  virtual ~pancake_thriftIf() {}

  virtual int64_t get_client_id() = 0;
  virtual void register_client_id(const int32_t block_id, const int64_t client_id) = 0;

  virtual void async_get(const sequence_id& seq_id, const std::string& key) = 0;
  virtual void async_put(const sequence_id& seq_id, const std::string& key, const std::string& value) = 0;
  virtual void async_get_batch(const sequence_id& seq_id, const std::vector<std::string>& keys) = 0;
  virtual void async_put_batch(const sequence_id& seq_id,
                               const std::vector<std::string>& keys,
                               const std::vector<std::string>& values) = 0;

  virtual void get(std::string& _return, const std::string& key) = 0;
  virtual void put(const std::string& key, const std::string& value) = 0;
  virtual void get_batch(std::vector<std::string>& _return, const std::vector<std::string>& keys) = 0;
  virtual void put_batch(const std::vector<std::string>& keys, const std::vector<std::string>& values) = 0;
};

class pancake_thrift_get_client_id_args {
public:
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_register_client_id_args {
public:
  pancake_thrift_register_client_id_args() : block_id(0), client_id(0) {}
  int32_t block_id;
  int64_t client_id;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_async_get_args {
public:
  sequence_id seq_id;
  std::string key;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_async_put_args {
public:
  sequence_id seq_id;
  std::string key;
  std::string value;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_async_get_batch_args {
public:
  sequence_id seq_id;
  std::vector<std::string> keys;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_async_put_batch_args {
public:
  sequence_id seq_id;
  std::vector<std::string> keys;
  std::vector<std::string> values;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_get_args {
public:
  std::string key;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_put_args {
public:
  std::string key;
  std::string value;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_get_batch_args {
public:
  std::vector<std::string> keys;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_put_batch_args {
public:
  std::vector<std::string> keys;
  std::vector<std::string> values;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_get_client_id_result {
public:
  pancake_thrift_get_client_id_result() : success(0), __isset_success(false) {}
  int64_t success;
  bool __isset_success;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_get_result {
public:
  pancake_thrift_get_result() : __isset_success(false) {}
  std::string success;
  bool __isset_success;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};

class pancake_thrift_get_batch_result {
public:
  pancake_thrift_get_batch_result() : __isset_success(false) {}
  std::vector<std::string> success;
  bool __isset_success;
  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;
};


class pancake_thriftProcessor : public ::apache::thrift::TDispatchProcessor {
public:
  explicit pancake_thriftProcessor(std::shared_ptr<pancake_thriftIf> iface);

protected:
  bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot,
                    ::apache::thrift::protocol::TProtocol* oprot,
                    const std::string& fname,
                    int32_t seqid,
                    void* callContext) override;

private:
  std::shared_ptr<pancake_thriftIf> iface_;
  typedef void (pancake_thriftProcessor::*ProcessFunction)(int32_t,
      ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;

  void process_get_client_id(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  void process_register_client_id(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  void process_async_get(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  void process_async_put(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  void process_async_get_batch(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  void process_async_put_batch(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  void process_get(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  void process_put(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  void process_get_batch(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  void process_put_batch(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
};

#endif
