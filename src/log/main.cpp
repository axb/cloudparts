#include "scoped_timer.h"
#include "stripe.h"
#include <iostream>

int main(int argc, char *argv[]) {
  Stripe::ptr_t st(new Stripe("./st1"));

  //
  // writing
  //
  {
    scoped_timer_t tm("writing");
    auto a = st->appender();

    cloudparts::log::data::LogRec lr;
    auto wr = std::back_inserter(a);
    for (int i = 0; i < 1005000; ++i) {
      lr.set_key("aaa");
      lr.set_msg("some msg");
      lr.set_partition(100);
      wr = lr;
    }
  }

  //
  // now reading
  //
  {
    scoped_timer_t tm("reading");
    for (auto it = st->lower_bound(1000000); it != st->end(); ++it) {
      std::cout << it->key() << std::endl;
    }
  }

  return 0;
}
