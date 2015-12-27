#include "db_impl.h"

namespace easydb {

Status DB::Open(const Options& options, const std::string& name, DB** dbptr)
{
    return Status::OK();
}



DBImpl::DBImpl(const Options& options, const std::string& dbname)
{
}

DBImpl::~DBImpl()
{
}

Status DBImpl::Put(const WriteOptions&, const Slice& key, const Slice& value)
{
    return Status::OK();
}

}
