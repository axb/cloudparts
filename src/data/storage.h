//
// Created by alexb_vi324bx on 23.01.2016.
//

#ifndef PROJECT_STORAGE_H
#define PROJECT_STORAGE_H

#include <functional>

/*
 * single partition manager
 *
 */
class storage {
public:
    void open();
    void close();

    void flush();

    stg_obj* root(std::string nm);

    //
    // guarded execution
    //
    // stg ( [=](){ ... your code...} );
    //
    void operator () ( std::function<void() >  todo ) {
        try {
            todo();
        } catch (...) {

        }
    }

    void operator () ( stg_obj* sync_root, std::function<void() >  todo ) {
        try {
            todo();
        } catch (...) {

        }
    }
};


#endif //PROJECT_STORAGE_H
