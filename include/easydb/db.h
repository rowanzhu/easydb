#ifndef EASYDB_INCLUDE_DB_H
#define EASYDB_INCLUDE_DB_H

#include <stdint.h>
#include <stdio.h>
#include <string>
#include "easydb/options.h"
#include "easydb/slice.h"
#include "easydb/status.h"

namespace easydb {

// A DB is a persistent ordered map from keys to values.
// A DB is safe for concurrent access from multiple threads without
// any external synchronization.
class DB {
 public:
  // Open the database with the specified "name".
  // Stores a pointer to a heap-allocated database in *dbptr and returns
  // OK on success.
  // Stores NULL in *dbptr and returns a non-OK status on error.
  // Caller should delete *dbptr when it is no longer needed.
  static Status Open(const Options& options,
                     const std::string& name,
                     DB** dbptr);

  DB(){}
  virtual ~DB(){}

  // Set the database entry for "key" to "value".  Returns OK on success,
  // and a non-OK status on error.
  virtual Status Put(const WriteOptions& options,
                     const Slice& key,
                     const Slice& value) = 0;

 private:
  // No copying allowed
  DB(const DB&);
  void operator=(const DB&);
};

} 

#endif
