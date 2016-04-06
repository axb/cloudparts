//
//  stripe.h
//  cloudparts
//
//  Created by axb on 05/03/16.
//

#ifndef stripe_h
#define stripe_h


///////////////////////////////////////////////////////////////////////////////////
//
// File based logs storage
//    Filename is like "<log_dir>/<stripe_id>/<starting_offset>.log"
//    File format = [(uint_32t)size LogEntry]
//
//
// Usage
//    --------  write data
//    auto wr = Stripe( stripe_dir ).writer();
//    wr( logRec ); -  write data

//    inserter() - end-pointing output iterator
//
//    --------  read data
//    begin() - all data iterator
//    begin(key) - given key data iterator
//    end(), end(key), rbegin(), rbegin(key) etc.
//
//    --------  housekeeping
//    collapseKeys()
//    reset() - start from beginning
//    cutTail( from_offset) - remove history
//
// TODO Features
//    lock on files for concurrent writes and reads from many processes
//    waiting on tail
//    consistency checks
//
///////////////////////////////////////////////////////////////////////////////////

#include "data.pb.h"

//class Stripe
//{
//public:
//};
//
#include <string>
#include <functional>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace bs = boost::iostreams;

class Stripe : public std::enable_shared_from_this< Stripe >
{
   std::string _id;
   
   uint64_t _lastOffset;
   
   void buildFrame();
   static const uint64_t FRAME_FILESIZE = 64 * 1024 * 1024; // 64 Mb
   bs::stream<bs::mapped_file_sink>  _frame;
   
   Stripe( std::string id_);
public:
   typedef std::shared_ptr<Stripe> TPtr;
   typedef std::function< uint64_t( uint64_t, std::string, std::string, std::string ) > TWriter;
   
   static TPtr create( std::string id_ ) { return TPtr{ new Stripe( id_ ) }; }
   
   ///////////////////////////////////////////////////////////////////////////////////
   //
   // writing facilities
   //
   ///////////////////////////////////////////////////////////////////////////////////
   
   //
   // usage:
   //    auto wr = storage.writer( "stripe_id", config );
   //    wr( ..... ); // write data
   //
   static TWriter writer( std::string id_ ) {
      auto st = create( id_);
      return [ st ] ( uint64_t offset, std::string key, std::string data, std::string localtime ) {
         return ( *st )( offset, key, data, localtime );
      };
   }
   
   //
   // for primary nodes
   // for replicas offset = UINT64_MAX
   //
   uint64_t operator() ( uint64_t offset, std::string key, std::string data, std::string localtime );
   
   ///////////////////////////////////////////////////////////////////////////////////
   //
   // reading facilities
   //
   ///////////////////////////////////////////////////////////////////////////////////
   
   // iterator operator[]( uint64_t offset )
   // iterator begin()
   // iterator end()
   // iterator rbegin()
   // iterator rend()
   
   virtual ~Stripe();
};


#endif /* stripe_h */
