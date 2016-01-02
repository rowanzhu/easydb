#include "merger.h"

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
    int ret = 0;
    ret = SelectFile();
    ret = MergeToMap();
    ret = WriteToFile();

    return ret;
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
    return 0;
}

}
