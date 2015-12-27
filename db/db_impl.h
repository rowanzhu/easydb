#ifndef EASYDB_DB_IMPL_H
#define EASYDB_DB_IMPL_H

#include "easydb/db.h"
#include <string>

namespace easydb {

class LogWriter;
class WritableFile;

class DBImpl : public DB {
 public:
  DBImpl(const Options& options, const std::string& dbname);
  virtual ~DBImpl();

  // Implementations of the DB interface
  virtual Status Put(const WriteOptions&, const Slice& key, const Slice& value);

  Status Recover();
 private:
  Options options_;
  std::string dbname_; //dir name
  LogWriter *p_log_writer_;
  WritableFile *p_writable_file_;

 private:
  // No copying allowed
  DBImpl(const DBImpl&);
  void operator=(const DBImpl&);
};

}  

#endif 
