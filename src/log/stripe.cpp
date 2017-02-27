#include "stripe.h"
#include <boost/filesystem.hpp>
#include <sstream>

namespace fs = boost::filesystem;
namespace ipc = boost::interprocess;

Stripe::Stripe(const std::string &path)
    : std::enable_shared_from_this<Stripe>() {
  _path = path;
}

uint64_t Stripe::offset() {
  uint64_t res = 0;

  fs::path dir(_path);
  for (fs::directory_entry &fe : fs::directory_iterator(dir)) {
    if (fs::is_regular_file(fe.path())) {
      try {
        uint64_t offset = std::stoull(fe.path().filename().string());
        if (offset > res)
          res = offset;
      } catch (...) {
      }
    }
  }

  /// @todo check "___current" file

  return res;
}

Stripe::input_iterator::input_iterator(Stripe::ptr_t st, uint64_t offset) {
  _st = st;
  _offset = offset;

  // if (_offset == M_TAIL)
  //   get last record
}

Stripe::input_iterator::input_iterator(const input_iterator &from) {
  _st = from._st;
  _offset = from._offset;
}

void Stripe::input_iterator::bind() {
  /// @todo bind to current file position
  // if (!mapping_ready) {
  //   find_file_by_offset, map it
  // }
  // find_by_offset
}

Stripe::input_iterator &Stripe::input_iterator::operator++(int steps) {
  // @todo optimize for large steps
  for (int k = 0; k < steps; ++k)
    operator++();
  return *this;
}

Stripe::input_iterator &Stripe::input_iterator::operator++() {
  /// @todo
  return *this;
}

bool Stripe::input_iterator::operator!=(const Stripe::input_iterator &other) {
  return _st == other._st && _offset == other._offset;
}

Stripe::input_iterator::reference Stripe::input_iterator::operator*() const {
  return _current;
}

const Stripe::input_iterator::pointer Stripe::input_iterator::
operator->() const {
  return &(operator*());
}

Stripe::input_iterator Stripe::begin() {
  return input_iterator(shared_from_this(), input_iterator::M_BEGIN);
}

Stripe::input_iterator Stripe::end() {
  return input_iterator(shared_from_this(), input_iterator::M_END);
}

Stripe::input_iterator Stripe::tail() {
  return input_iterator(shared_from_this(), input_iterator::M_TAIL);
}

Stripe::input_iterator Stripe::lower_bound(uint64_t offset) {
  // @fixme - find lower bound
  input_iterator it(shared_from_this(), offset);
  return it;
} //

Stripe::back_inserter_adapter Stripe::appender() {
  return back_inserter_adapter(shared_from_this());
}

Stripe::back_inserter_adapter::back_inserter_adapter(Stripe::ptr_t ps) {
  _st = ps;
}

Stripe::back_inserter_adapter::~back_inserter_adapter() { finishStream(); }

bool Stripe::back_inserter_adapter::openStream() {
  // build filename
  std::stringstream dir;
  dir << _st->path() << "/";
  fs::create_directories(dir.str());

  // find current offset if not set
  _cursor.offset = _st->offset();

  // create current file
  dir << "___current";
  {
    ipc::file_mapping::remove(dir.str().c_str());
    std::filebuf fbuf;
    fbuf.open(dir.str().c_str(),
              std::ios_base::in | std::ios_base::out | std::ios_base::trunc |
                  std::ios_base::binary);
    fbuf.pubseekoff(Stripe::SEGMENT_SIZE - 1, std::ios_base::beg);
    fbuf.sputc(0);
  }

  // memmap to array
  _cursor.mappedFile = ipc::file_mapping(dir.str().c_str(), ipc::read_write);
  _cursor.mappedRegion =
      ipc::mapped_region(_cursor.mappedFile, ipc::read_write);
  _cursor.stream.reset(new google::protobuf::io::ArrayOutputStream(
      _cursor.mappedRegion.get_address(), SEGMENT_SIZE));

  return true;
}

void Stripe::back_inserter_adapter::finishStream() {
  //
  // put end-marker
  //
  google::protobuf::io::CodedOutputStream output(_cursor.stream.get());
  output.WriteVarint32(0);

  // close file
  _cursor.reset();

  //
  // rename file to last_offset
  //
  std::stringstream nm;
  nm << _st->path() << "/" << _cursor.offset;
  fs::rename(_st->path() + "/___current", nm.str());
}

void Stripe::back_inserter_adapter::push_back(const value_type &rec) {
  if (!_cursor.stream) {
    openStream();
  }
  if (rec.ByteSize() + _cursor.stream->ByteCount() + 33 > SEGMENT_SIZE) {
    finishStream();
    openStream();
  }

  /// @todo write delimited data
  ++_cursor.offset;
  {
    google::protobuf::io::CodedOutputStream output(_cursor.stream.get());

    // write the size
    const int size = rec.ByteSize();
    output.WriteVarint32(size);

    // write data
    uint8_t *buffer = output.GetDirectBufferForNBytesAndAdvance(size);
    if (buffer != NULL) {
      rec.SerializeWithCachedSizesToArray(buffer);
    } else {
      rec.SerializeWithCachedSizes(&output);
      if (output.HadError()) {
        /// @todo
      }
    }
  }
}

//
// bool readDelimitedFrom(
//                       google::protobuf::io::ZeroCopyInputStream* rawInput,
//                       google::protobuf::MessageLite* message) {
//   // We create a new coded stream for each message.  Don't worry, this is
//   fast,
//   // and it makes sure the 64MB total size limit is imposed per-message
//   rather
//   // than on the whole stream.  (See the CodedInputStream interface for more
//   // info on this limit.)
//   google::protobuf::io::CodedInputStream input(rawInput);
//
//   // Read the size.
//   uint32_t size;
//   if (!input.ReadVarint32(&size)) return false;
//
//   // Tell the stream not to read beyond that size.
//   google::protobuf::io::CodedInputStream::Limit limit =
//   input.PushLimit(size);
//
//   // Parse the message.
//   if (!message->MergeFromCodedStream(&input)) return false;
//   if (!input.ConsumedEntireMessage()) return false;
//
//   // Release the limit.
//   input.PopLimit(limit);
//
//   return true;
//}
