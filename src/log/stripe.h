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
//    Filename is like "<log_dir>/<stripe_id>/<last_offset>" or "___current" for last file
//
//
// Usage
//    --------  write data
//    auto wr = Stripe( stripe_dir ).writer();
//    wr( logRec ); -  write data
//
//    writer() - end-pointing output iterator
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
/// @todo Features
//    lock on files for concurrent writes and reads from many processes
//    waiting on tail
//    consistency checks
//
///////////////////////////////////////////////////////////////////////////////////

#include "data.pb.h"

#include <string>
#include <memory>
#include <functional>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>


class Stripe : public std::enable_shared_from_this<Stripe> {
   std::string _path;
public:
   
   static const uint64_t SEGMENT_SIZE =  1 *1024 * 1024;
   
   std::string path() const {
      return _path;
   }
   
   uint64_t offset();
   
   typedef std::shared_ptr<Stripe> ptr_t;
   
   Stripe( const std::string& path );
   
   //
   // reader
   //
   class input_iterator : public std::iterator< std::forward_iterator_tag, cloudparts::log::data::LogRec > {
      /// @todo
      //  bool _end, _tail;
      //
      //      std::string _key;
      //      pointer _rec;
      mutable cloudparts::log::data::LogRec _current;
      void bind() {}
      
   public:
      input_iterator& operator++(int steps) {
         /// @todo
         return *this;
      }
      input_iterator& operator++() {
         /// @todo
         return *this;
      }
      bool operator != (const input_iterator& other ) {
         /// @todo
         return false;
      }
      reference operator*() const {
         /// @todo
         return _current;
      }
      const pointer operator->() const {
         return &(operator*());
      }
   };
   
   input_iterator begin(); // begin of history
   input_iterator end();   // end-marker
   input_iterator tail();  // live-end iterator
   input_iterator lower_bound( uint64_t offset ); //
   
   //
   // writer
   //
   class back_inserter_adapter {
      
      Stripe::ptr_t _st;
      
      struct  TCursor {
         std::unique_ptr<google::protobuf::io::ArrayOutputStream>  stream;
         boost::interprocess::file_mapping         mappedFile;
         boost::interprocess::mapped_region        mappedRegion;
         uint64_t                                  offset;
         
         TCursor() {
            reset();
         }
         TCursor( const TCursor& other ) {
            reset();
            offset = other.offset;
         }
         ~TCursor() {
            reset();
         }
         void reset() {
            namespace ipc = boost::interprocess;
            stream.release();
            mappedRegion.flush();
            ipc::mapped_region tmr;
            mappedRegion.swap( tmr );
            ipc::file_mapping tmf;
            mappedFile.swap( tmf );
         }
      } _cursor;
      
      bool openStream();
      void finishStream();
      
   public:
      back_inserter_adapter( Stripe::ptr_t ps );
      ~back_inserter_adapter();
      
      /// interface for std::back_inserter
      typedef cloudparts::log::data::LogRec value_type;
      void push_back( const value_type& rec );
   };
   back_inserter_adapter appender();
};

#endif /* stripe_h */
