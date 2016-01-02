#include <cstdio>
#include "easydb/db.h"
#include "db/file.h"

using namespace easydb;

int main()
{
    DB *p_db = NULL;
    Options opt;
    //opt.max_log_file_size = 100;
    Status s = DB::Open(opt, "./db_test", &p_db);

    if(NULL == p_db)
    {
        printf("open db error|%s\n", s.ToString().c_str());
        return 0;
    }

    for(int i = 0; i < 10000; ++i)
    {
        char buff[16] = {0};
        snprintf(buff, sizeof(buff), "%d", i);
        s = p_db->Put(WriteOptions(), buff, "hello,world");
        if(!s.ok())
        {
            printf("put error|%s\n", s.ToString().c_str());
            return 0;
        }
    }

    delete p_db;

    return 0;
}

/*int main(int argc, char *argv[])
{
    std::string dir_name(argv[1]);
    
    std::vector<std::string> file_list;
    ReadDir(dir_name, &file_list);

    for(int i = 0; i < file_list.size(); ++i)
    {
        if(file_list[i] == "." || file_list[i] == "..")
        {
            continue;
        }
        
        printf("%s\n", file_list[i].c_str());

        FileType type;
        std::vector<uint32_t> vec_num;
        ParseFileName(file_list[i], &type, &vec_num);
        
        printf("type:%d\n", type);
        for(int i = 0; i < vec_num.size(); ++i)
        {
            printf("num:%u\n", vec_num[i]);
        }
    }
    
    return 0;
}*/