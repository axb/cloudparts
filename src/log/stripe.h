#ifndef stripe_h
#define stripe_h

#include "data.pb.h"

#include <functional>
#include <memory>
#include <string>

#include <Avro/DataFile.hh>

///////////////////////////////////////////////////////////////////////////////////
//
// File based logs storage
//    Filename is like "<log_dir>/<stripe_id>/<last_offset>" or "___current" for
//    last file
//
// @todo
//    use Avro DataFile
// Features
//    lock on files for concurrent writes and reads from many processes
//          (writer-pid-file in directory or lock on ___current-file)
//    waiting on tail
//    consistency checks
//    file-like API - open, read, write, close
//    + boost::asio support for tail iterator
//
///////////////////////////////////////////////////////////////////////////////////

class Stripe : public std::enable_shared_from_this<Stripe> {
  std::string _path;

public:
  static const uint64_t SEGMENT_SIZE =
      1 * 1024 * 1024; // 1Mb - small number actually
  typedef std::shared_ptr<Stripe> ptr_t;
  Stripe(const std::string &path);

  std::string path() const { return _path; }
  uint64_t offset();

  //
  // reader
  //
  class input_iterator : public std::iterator<std::forward_iterator_tag,
                                              cloudparts::log::data::LogRec> {
    friend class Stripe;
    input_iterator(Stripe::ptr_t st, uint64_t offset);

    Stripe::ptr_t _st;
    // special values:
    static const uint64_t M_BEGIN = 0;
    static const uint64_t M_END = UINT64_MAX;
    static const uint64_t M_TAIL = UINT64_MAX - 1;
    uint64_t _offset;
    mutable cloudparts::log::data::LogRec _current;
    void bind();

  public:
    input_iterator(const input_iterator &from);
    input_iterator &operator++(int steps);
    input_iterator &operator++();
    bool operator!=(const input_iterator &other);
    reference operator*() const;
    const pointer operator->() const;
  };

  input_iterator begin(); // begin of history
  input_iterator end();   // end-marker
  input_iterator tail();  // live-end iterator (++ does not make it "end" but
                          // locks or does nothing when on the last record)
  input_iterator lower_bound(uint64_t offset); //

  //
  // writer
  // @todo lock pid-file
  //
  class back_inserter_adapter {

    Stripe::ptr_t _st;

    struct cursor_t {
      uint64_t offset;

      cursor_t() {}
      cursor_t(const cursor_t &other) {
        reset();
        offset = other.offset;
      }
      ~cursor_t() { reset(); }
      void reset() {}
    } _cursor;

    bool openStream();
    void finishStream();

  public:
    back_inserter_adapter(Stripe::ptr_t ps);
    ~back_inserter_adapter();

    /// interface for std::back_inserter
    typedef cloudparts::log::data::LogRec value_type;
    void push_back(const value_type &rec);
  };
  back_inserter_adapter appender();
};

#endif /* stripe_h */
