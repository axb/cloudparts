//
//  stripe.cpp
//  cloudparts
//
//  Created by axb on 05/03/16.
//
//


//CodedOutputStream writer = CodedOutputStream.newInstance(outputStreamToWrite);
//writer.writeRawVarint32(bytes.length);
//writer.writeRawBytes(bytes);
//
//To read the entire file:
//CodedInputStream is = CodedInputStream.newInstance(inputStreamToWrap);
//while (!is.isAtEnd()) {int size = is.readRawVarint32(); YourMessage.parseFrom(is.readRawBytes(size);}





//bool writeDelimitedTo(
//                      const google::protobuf::MessageLite& message,
//                      google::protobuf::io::ZeroCopyOutputStream* rawOutput) {
//   // We create a new coded stream for each message.  Don't worry, this is fast.
//   google::protobuf::io::CodedOutputStream output(rawOutput);
//   
//   // Write the size.
//   const int size = message.ByteSize();
//   output.WriteVarint32(size);
//   
//   uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
//   if (buffer != NULL) {
//      // Optimization:  The message fits in one buffer, so use the faster
//      // direct-to-array serialization path.
//      message.SerializeWithCachedSizesToArray(buffer);
//   } else {
//      // Slightly-slower path when the message is multiple buffers.
//      message.SerializeWithCachedSizes(&output);
//      if (output.HadError()) return false;
//   }
//   
//   return true;
//}
//
//bool readDelimitedFrom(
//                       google::protobuf::io::ZeroCopyInputStream* rawInput,
//                       google::protobuf::MessageLite* message) {
//   // We create a new coded stream for each message.  Don't worry, this is fast,
//   // and it makes sure the 64MB total size limit is imposed per-message rather
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

#include "stripe.h"
#include <sstream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

void Stripe::buildFrame() {
   //
   // build filename
   //
   std::stringstream dir;
   dir << "." << "/" << _id << "/"; // TODO: use root from cofig
   fs::create_directories(dir.str());
   
   int ix = 0;
   std::time_t tm = std::time(0);
   std::string sfn;
   do {
      std::stringstream fn;
      fn << dir.str() << tm << "_" << ix << ".log";
      ++ix;
      sfn = fn.str();
   } while (fs::exists(sfn));
   
   //
   // open mapped file
   //
   if (_frame.is_open())
      _frame.close();
   
   bs::mapped_file_params args;
   args.path = sfn;
   args.new_file_size = FRAME_FILESIZE;
   _frame.open(bs::mapped_file_sink(args));
}

Stripe::Stripe(std::string id_) : _id(id_), _lastOffset(1) {
   /// <todo> read last offset from storage
}

uint64_t Stripe::operator() (uint64_t offset, std::string key, std::string data, std::string localtime) {
   if ( offset == UINT64_MAX )
      offset = ++_lastOffset;
   
//   LogEntry le{ offset, key, data, localtime };
//   if (le.size() >= FRAME_FILESIZE)
//      return false; // no way to write that big thing - "нельзя впихнуть невпихуемое"
//   
//   // let it be 2 lines - we can not OR'em
//   if (!_frame.is_open())                                      buildFrame();
//   if ((size_t)_frame.tellp() + le.size() >= FRAME_FILESIZE)   buildFrame();
//   
//   // finally write
//   _frame << le;
   
   return offset;
}

Stripe::~Stripe() {
   if (_frame.is_open()) {
      std::flush(_frame);
      _frame.close();
   }
}
