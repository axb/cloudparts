//
//  stripe.cpp
//  cloudparts
//
//  Created by axb on 05/03/16.
//
//

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
