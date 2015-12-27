#ifndef EASYDB_LOG_WRITER_H
#define EASYDB_LOG_WRITER_H

#include <stdint.h>
#include "log_format.h"
#include "easydb/slice.h"
#include "easydb/status.h"

namespace easydb {

class WritableFile;

class LogWriter {
 public:
  // Create a writer that will append data to "*dest".
  // "*dest" must be initially empty.
  // "*dest" must remain live while this LogWriter is in use.
  explicit LogWriter(WritableFile* dest);
  ~LogWriter();

  Status AddRecord(const Slice& slice);

 private:
  WritableFile* dest_;
  int block_offset_;       // Current offset in block

  // crc32c values for all supported record types.  These are
  // pre-computed to reduce the overhead of computing the crc of the
  // record type stored in the header.
  uint32_t type_crc_[kMaxRecordType + 1];

  Status EmitPhysicalRecord(RecordType type, const char* ptr, size_t length);

  // No copying allowed
  LogWriter(const LogWriter&);
  void operator=(const LogWriter&);
};

}

#endif
