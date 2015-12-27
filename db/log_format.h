#ifndef EASYDB_LOG_FORMAT_H
#define EASYDB_LOG_FORMAT_H

namespace easydb {

enum ValueType 
{
    kTypeDeletion = 0x0,
    kTypeValue = 0x1 
};

enum RecordType {
  // Zero is reserved for preallocated files
  kZeroType = 0,

  kFullType = 1,

  // For fragments
  kFirstType = 2,
  kMiddleType = 3,
  kLastType = 4
};
static const int kMaxRecordType = kLastType;

static const int kBlockSize = 32768;

// Header is checksum (4 bytes), length (2 bytes), type (1 byte).
static const int kHeaderSize = 4 + 2 + 1;

}

#endif 
