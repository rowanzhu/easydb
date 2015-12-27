#include "db_impl.h"
#include "log_writer.h"
#include "file.h"
#include "coding.h"

namespace easydb {

#define DB_FILE_NAME_LOG_0 "0.log"

Status DB::Open(const Options& options, const std::string& dbname, DB** dbptr)
{
    *dbptr = NULL;

    DBImpl* impl = new DBImpl(options, dbname);
    Status s = impl->Recover();
    if (s.ok()) 
    {
        *dbptr = impl;
    }else
    {
        delete impl;
    }

    return s;
}


    
DBImpl::DBImpl(const Options& options, const std::string& dbname)
    :options_(options),
    dbname_(dbname),
    p_log_writer_(NULL),
    p_writable_file_(NULL)
{
}

DBImpl::~DBImpl()
{
    if(NULL != p_log_writer_)
    {
        delete p_log_writer_;
    }

    if(NULL != p_writable_file_)
    {
        delete p_writable_file_;
    }
}

Status DBImpl::Put(const WriteOptions&, const Slice& key, const Slice& value)
{
    std::string str_record;
    str_record.push_back(static_cast<char>(kTypeValue));
    PutLengthPrefixedSlice(&str_record, key);
    PutLengthPrefixedSlice(&str_record, value);

    Status s = p_log_writer_->AddRecord(str_record);
    
    return s;
}

Status DBImpl::Recover()
{
    if(!FileExists(dbname_))
    {
        Status s = CreateDir(dbname_);
        if(!s.ok())
        {
            return s;
        }
    }
    
    std::string str_full_log_0_name(dbname_);
    str_full_log_0_name.append("/");
    str_full_log_0_name.append(DB_FILE_NAME_LOG_0);

    p_writable_file_ = new WritableFile(str_full_log_0_name);
    if(!p_writable_file_->IsValid())
    {
        return Status::IOError(str_full_log_0_name, "invalid fd");
    }

    p_log_writer_ = new LogWriter(p_writable_file_);
    
    return Status::OK();
}



}
