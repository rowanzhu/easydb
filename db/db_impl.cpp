#include "db_impl.h"
#include "log_writer.h"
#include "file.h"
#include "coding.h"
#include "log_reader.h"
#include "util.h"

namespace easydb {

#define DB_FILE_NAME_LOG_0 "0.log"

Status GetAllFromOneFile(const std::string &file_name, 
        std::map<std::string, std::string> &map_all_kv)
{
    Status s;
    
    SequentialFile log_file(file_name);
    if(!log_file.IsValid())
    {
        return Status::IOError(file_name, "invalid fd");
    }

    LogReader::Reporter reporter;

    LogReader log_reader(&log_file, &reporter, true, 0);

    std::string scratch;
    Slice record;
    Slice key;
    Slice value;
    while (log_reader.ReadRecord(&record, &scratch))
    {
        char tag = record[0];
        record.remove_prefix(1);

        switch(tag)
        {
            case kTypeValue:
            {
                if(GetLengthPrefixedSlice(&record, &key) &&
                        GetLengthPrefixedSlice(&record, &value))
                {
                    map_all_kv[key.ToString()] = value.ToString();
                }
            }
            break;
            case kTypeDeletion:
            {
                //TODO:
            }
            break;
            default:
            {
                //return Status::Corruption("unknown record tag");
            }
        }
    }

    return s;
}


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
    p_writable_file_(NULL),
    max_filename_num_(0),
    last_file_sync_ts_(0)
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

#ifdef EASYDB_FILE_SYNC_ON_DEMAND
    time_t now = Now();
    if(now - last_file_sync_ts_  >= options_.file_sync_interval &&
            s.ok())
    {
        s = p_writable_file_->Sync();
        last_file_sync_ts_ = now;
    }
#endif

    if(p_writable_file_->GetFileSize() > options_.max_log_file_size)
    {
        std::string str_full_log_0_name(p_writable_file_->GetFileName());
        //release 0.log
        delete p_log_writer_;
        p_log_writer_ = NULL;
        delete p_writable_file_;
        p_writable_file_ = NULL;
        
        //rename 0.log
        std::string new_logfile_name;
        GetNewLogFileName(new_logfile_name);
        GetFullFileName(new_logfile_name);
        s = RenameFile(str_full_log_0_name, new_logfile_name);
        if(!s.ok())
        {
            return s;
        }

        //new 0.log
        p_writable_file_ = new WritableFile(str_full_log_0_name);
        if(!p_writable_file_->IsValid())
        {
            return Status::IOError(str_full_log_0_name, "invalid fd");
        }

        p_log_writer_ = new LogWriter(p_writable_file_);
    }
    
    return s;
}

Status DBImpl::GetAll(std::map<std::string, std::string> &map_all_kv)
{
    map_all_kv.clear();
    Status s;

    std::vector<std::string> file_list;
    ReadDir(dbname_, &file_list);

    std::map< uint32_t, std::string* > sorted_file_map;
    for(int i = 0; i < file_list.size(); ++i)
    {
        if(file_list[i] == "." || 
                file_list[i] == ".." ||
                file_list[i] == DB_FILE_NAME_LOG_0)
        {
            continue;
        }
        
        FileType file_type = kUnknownFile;
        std::vector<uint32_t> vec_num;
        ParseFileName(file_list[i], &file_type, &vec_num);

        switch(file_type)
        {
            case kLogFile:
            {
                //SetMaxFilenameNum
                if(vec_num.size() > 0)
                {
                    sorted_file_map[vec_num[0]] = &(file_list[i]);
                }
            }
            break;
            default:
            {
            }
        }
    }

    std::map< uint32_t, std::string* >::iterator iter;
    for(iter = sorted_file_map.begin(); iter != sorted_file_map.end(); ++iter)
    {
        std::string *p_str_file_name = iter->second;
        GetFullFileName(*p_str_file_name);
        GetAllFromOneFile(*p_str_file_name, map_all_kv);
    }
    
    std::string str_full_log_0_name(DB_FILE_NAME_LOG_0);
    GetFullFileName(str_full_log_0_name);
    GetAllFromOneFile(str_full_log_0_name, map_all_kv);

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
    else
    {
        ScanDir();
    }
    
    std::string str_full_log_0_name(DB_FILE_NAME_LOG_0);
    GetFullFileName(str_full_log_0_name);

    uint64_t file_size = 0;
    GetFileSize(str_full_log_0_name, &file_size);
    if(file_size > 0)
    {
        //TODO: only rename
        std::string new_logfile_name;
        GetNewLogFileName(new_logfile_name);
        GetFullFileName(new_logfile_name);
        Status s = RenameFile(str_full_log_0_name, new_logfile_name);
        if(!s.ok())
        {
            return s;
        }
    }

    p_writable_file_ = new WritableFile(str_full_log_0_name);
    if(!p_writable_file_->IsValid())
    {
        return Status::IOError(str_full_log_0_name, "invalid fd");
    }

    p_log_writer_ = new LogWriter(p_writable_file_);
    
    return Status::OK();
}

void DBImpl::ScanDir()
{
    std::vector<std::string> file_list;
    ReadDir(dbname_, &file_list);

    for(int i = 0; i < file_list.size(); ++i)
    {
        if(file_list[i] == "." || file_list[i] == "..")
        {
            continue;
        }
        
        FileType file_type = kUnknownFile;
        std::vector<uint32_t> vec_num;
        ParseFileName(file_list[i], &file_type, &vec_num);

        switch(file_type)
        {
            case kLogFile:
            {
                //SetMaxFilenameNum
                if(vec_num.size() > 0 &&
                        vec_num[0] > max_filename_num_)
                {
                    max_filename_num_ = vec_num[0];
                }
            }
            break;
            case kMergingFile:
            {
                //TODO: DeleteInvalidFile
            }
            break;
            case kMergedFile:
            {
                //TODO: DeleteInvalidFile & rename
            }
            break;
            default:
            {
            }
        }
    }
}

void DBImpl::GetNewLogFileName(std::string &ret)
{
    char buff[256] = {0};

    ++max_filename_num_;
    snprintf(buff, sizeof(buff), "%u.log", max_filename_num_);
    ret.assign(buff);
}

void DBImpl::GetFullFileName(std::string &file_name)
{
    std::string ret;
    ret.append(dbname_);
    ret.append("/");
    ret.append(file_name);

    file_name.assign(ret);
}

}
