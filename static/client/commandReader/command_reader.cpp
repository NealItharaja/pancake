#include "command_reader.h"
#include <thrift/protocol/TProtocol.h>
#include <thrift/TApplicationException.h>
#include "thrift_response_service.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;

command_reader::command_reader(std::shared_ptr<TProtocol> protocol) 
    : protocol_(std::move(protocol)), iprot_(protocol_.get()) {}

int64_t command_reader::recv_response(std::vector<std::string>& out) {
    std::string fname;
    TMessageType mtype;
    int32_t rseqid = 0;
    iprot_->readMessageBegin(fname, mtype, rseqid);

    if (mtype == T_EXCEPTION) {
        TApplicationException x;
        x.read(iprot_);
        iprot_->readMessageEnd();
        iprot_->getTransport()->readEnd();
        throw x;
    }

    pancake_thrift_response_async_response_args result;
    result.read(iprot_);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();

    if (result.__isset.seq_id && result.__isset.result) {
        out = result.result;
        return result.seq_id.seq;
    }

    return -1;
}
