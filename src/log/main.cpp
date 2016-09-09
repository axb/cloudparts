//
// Created by alexb_vi324bx on 22.01.2016.
//

#include "stripe.h"
#include <iostream>

int main(int argc, char* argv[])
{
   Stripe::ptr_t   st ( new Stripe("./st1") );
   
   
   //
   // test with Array...Stream over memmapped files
   //
   {
      auto a = st->appender();
      
      cloudparts::log::data::LogRec lr;
      auto wr = std::back_inserter( a );
      for ( int i = 0; i < 1005000; ++i ) {
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
      for ( auto it = st->lower_bound(1000000); it != st->end(); ++it ) {
         std::cout << it->key() << std::endl;
      }
   }
   
   return 0;
}