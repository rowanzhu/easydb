#include "merger.h"
#include "db/db_impl.h"
#include "db/util.h"
#include "db/file.h"
#include "db/log_writer.h"
#include "db/log_reader.h"
#include "db/coding.h"

namespace easydb
{

Merger::Merger(const std::string& dbname)
    :dbname_(dbname)
{

}

Merger::~Merger()
{

}

int Merger::Merge()
{
    if(0 != SelectFile())
    {
        return -1;
    }
    
    if(map_selected_file_.size() < 2)
    {
        return 0;
    }
    
    if(0 != MergeToMap())
    {
        return -2;
    }
    
    if(0 != WriteToFile())
    {
        return -3;
    }

    return 0;
}

int Merger::SelectFile()
{
    std::vector<std::string> file_list;
    ReadDir(dbname_, &file_list);

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
                if(vec_num.size() > 0)
                {
                    map_selected_file_[vec_num[0]] = file_list[i];
                }
            }
            break;
            default:
            {
            }
        }
    }
 
    return 0;   
}

int Merger::MergeToMap()
{
    std::map<uint32_t, std::string>::iterator iter;
    for(iter = map_selected_file_.begin(); iter != map_selected_file_.end(); ++iter)
    {
        std::string &str_file_name = iter->second;
        GetFullFileName(dbname_, str_file_name);
        GetAllFromOneFile(str_file_name, map_kv_);
    }
    
    return 0;
}

int Merger::WriteToFile()
{
    //1. write to mgi file
    std::string full_mgi_name;
    GetMergeFileName(full_mgi_name, "mgi");

    //scope for release
    {
        WritableFile writable_file(full_mgi_name);
        if(!writable_file.IsValid())
        {
            printf("open file failed|%s\n", full_mgi_name.c_str());
            return -1;
        }
        LogWriter log_writer(&writable_file);

        std::map<std::string, std::string>::iterator iter;
        for(iter = map_kv_.begin(); iter != map_kv_.end(); ++iter)
        {
            std::string str_record;
            str_record.push_back(static_cast<char>(kTypeValue));
            PutLengthPrefixedSlice(&str_record, iter->first);
            PutLengthPrefixedSlice(&str_record, iter->second);

            Status s = log_writer.AddRecord(str_record);
            if(!s.ok())
            {
                printf("Merger::WriteToFile|%s\n", s.ToString().c_str());
                return -1;
            }
        }

        writable_file.Sync();
    }

    //2. rename mgi to mgd
    std::string full_mgd_name;
    GetMergeFileName(full_mgd_name, "mgd");
    RenameFile(full_mgi_name, full_mgd_name);

    //3. delete old log file
    std::map<uint32_t, std::string>::iterator iter;
    for(iter = map_selected_file_.begin(); iter != map_selected_file_.end(); ++iter)
    {
        DeleteFile(iter->second);
    }

    //4. rename mgd to min.log
    std::string full_log_name;
    GetLogFileName(full_log_name);
    RenameFile(full_mgd_name, full_log_name);

    return 0;
}

void Merger::GetMergeFileName(std::string &full_name, const std::string &suffix)
{
    full_name.clear();
    uint32_t min_num = map_selected_file_.begin()->first;
    uint32_t max_num = (--map_selected_file_.end())->first;

    char buff[256] = {0};
    snprintf(buff, sizeof(buff), "%u_%u.%s", min_num, max_num, suffix.c_str());

    full_name.assign(buff);
    GetFullFileName(dbname_, full_name);
}

void Merger::GetLogFileName(std::string &full_name)
{
    full_name.clear();
    uint32_t min_num = map_selected_file_.begin()->first;

    char buff[256] = {0};
    snprintf(buff, sizeof(buff), "%u.log", min_num);

    full_name.assign(buff);
    GetFullFileName(dbname_, full_name);
}

}
