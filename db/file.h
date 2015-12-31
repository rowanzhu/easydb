#ifndef EASYDB_FILE_H
#define EASYDB_FILE_H

#include <cstdio>
#include <string>
#include <vector>
#include "easydb/status.h"

namespace easydb
{

class Slice;

class SequentialFile {
 public:
  SequentialFile(const std::string& fname);
  virtual ~SequentialFile();

  bool IsValid();

  // Read up to "n" bytes from the file.  "scratch[0..n-1]" may be
  // written by this routine.  Sets "*result" to the data that was
  // read (including if fewer than "n" bytes were successfully read).
  // May set "*result" to point at data in "scratch[0..n-1]", so
  // "scratch[0..n-1]" must be live when "*result" is used.
  // If an error was encountered, returns a non-OK status.
  //
  // REQUIRES: External synchronization
  Status Read(size_t n, Slice* result, char* scratch);

  // Skip "n" bytes from the file. This is guaranteed to be no
  // slower that reading the same data, but may be faster.
  //
  // If end of file is reached, skipping will stop at the end of the
  // file, and Skip will return OK.
  //
  // REQUIRES: External synchronization
  Status Skip(uint64_t n);
 private:
  std::string filename_;
  FILE* file_;

 private:
  // No copying allowed
  SequentialFile(const SequentialFile&);
  void operator=(const SequentialFile&);
};

class WritableFile {
 public:
  WritableFile(const std::string& fname);
  virtual ~WritableFile();

  bool IsValid();
  uint64_t GetFileSize();
  const std::string &GetFileName();

  Status Append(const Slice& data);
 private:
  std::string filename_;
  int fd_;

 private:
  // No copying allowed
  WritableFile(const WritableFile&);
  void operator=(const WritableFile&);
};

class Logger {
 public:
  Logger(const std::string& fname);
  virtual ~Logger();

  // Write an entry to the log file with the specified format.
  void Logv(const char* format, va_list ap);
 private:
    FILE* file_;

 private:
  // No copying allowed
  Logger(const Logger&);
  void operator=(const Logger&);
};

enum FileType {
    kUnknownFile = 0,
    kLogFile,      //num.log
    kMergingFile,  //num_num_num[_num_...].mgi
    kMergedFile    //num_num_num[_num_...].mgd
};


Status CreateDir(const std::string& name);
bool FileExists(const std::string& fname);
Status ReadDir(const std::string& dir, std::vector<std::string>* result);
Status DeleteFile(const std::string& fname);
Status GetFileSize(const std::string& fname, uint64_t* size);
Status RenameFile(const std::string& src, const std::string& target);
int ParseFileName(const std::string& fname, FileType* type, std::vector<uint32_t> *vec_num);

}

#endif
