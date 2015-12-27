#ifndef EASYDB_DB_IMPL_H
#define EASYDB_DB_IMPL_H

#include "easydb/db.h"

namespace easydb {

class DBImpl : public DB {
 public:
  DBImpl(const Options& options, const std::string& dbname);
  virtual ~DBImpl();

  // Implementations of the DB interface
  virtual Status Put(const WriteOptions&, const Slice& key, const Slice& value);
  
 private:
  // No copying allowed
  DBImpl(const DBImpl&);
  void operator=(const DBImpl&);
};

}  

#endif 
