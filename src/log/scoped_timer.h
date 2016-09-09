//
//  scoped_timer.h
//  cloudparts
//
//  Created by axb on 09/09/2016.
//
//

#ifndef scoped_timer_h
#define scoped_timer_h

#include <iostream>
#include <chrono>

class scoped_timer_t
{
   std::string _caption;
   std::chrono::high_resolution_clock::time_point _start;
   int _count;
public:
   scoped_timer_t( std::string caption ) : _caption( caption ), _count( -1 ) {
      _start = std::chrono::high_resolution_clock::now();
      std::cout << std::endl << _caption << " ------------ " << std::endl;
   }
   
   void setCount( int  cnt ) {
      _count = cnt;
   }
   
   ~scoped_timer_t() {
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> diff = end - _start;
      std::cout << std::endl << "took : " << diff.count() << " seconds" << std::endl;
      if ( _count != -1 ) {
         std::cout << "made : " << _count << " records, " << std::endl
         << "perf : " << _count / diff.count() << " recs/sec " << std::endl;
      }
   }
};



#endif /* scoped_timer_h */
