#ifndef EASYDB_MERGE_TOOL_MERGER_H
#define EASYDB_MERGE_TOOL_MERGER_H

#include <string>
#include <map>
#include <cstdint>

namespace easydb
{

class Merger
{
 public:
     Merger(const std::string& dbname);
     virtual ~Merger();

     int Merge();
 private:
     int SelectFile();
     int MergeToMap();
     int WriteToFile();

     void GetMergeFileName(std::string &full_name, const std::string &suffix);
     void GetLogFileName(std::string &full_name);
 private:
     std::string dbname_;
     std::map<uint32_t, std::string> map_selected_file_;
     std::map<std::string, std::string> map_kv_;
};

}

#endif
