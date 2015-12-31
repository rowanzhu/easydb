#ifndef EASYDB_INCLUDE_OPTIONS_H
#define EASYDB_INCLUDE_OPTIONS_H

#include <stddef.h>

namespace easydb {

// Options to control the behavior of a database (passed to DB::Open)
struct Options {
    uint64_t max_log_file_size;

    Options()
        :max_log_file_size(104857600) //100M
    {
    }
};

// Options that control read operations
struct ReadOptions {
  
};

// Options that control write operations
struct WriteOptions {
  
};

} 

#endif
