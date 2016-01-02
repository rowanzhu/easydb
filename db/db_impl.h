#ifndef EASYDB_DB_IMPL_H
#define EASYDB_DB_IMPL_H

#include "easydb/db.h"
#include <string>
#include <ctime>

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
  uint32_t max_filename_num_;
  time_t last_file_sync_ts_;

 private:
  // No copying allowed
  DBImpl(const DBImpl&);
  void operator=(const DBImpl&);

  void ScanDir();
  void GetNewLogFileName(std::string &ret);
  void GetFullFileName(std::string &file_name);
};

}  

#endif 
