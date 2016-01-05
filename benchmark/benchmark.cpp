#include <cstdio>
#include "easydb/db.h"
#include "db/util.h"

using namespace easydb;

int BenchmarkWrite()
{
    DB *p_db = NULL;
    Options opt;
    //opt.max_log_file_size = 100;
    opt.file_sync_interval = 0;
    Status s = DB::Open(opt, "./db_test", &p_db);

    if(NULL == p_db)
    {
        printf("open db error|%s\n", s.ToString().c_str());
        return 0;
    }

    uint64_t begin = NowMicros();

    for(int i = 0; i < 10000; ++i)
    {
        char key[64] = {0};
        char value[256] = {0};
        snprintf(key, sizeof(key), "%d", i);
        snprintf(value, sizeof(value), "%d", i);
        s = p_db->Put(WriteOptions(), 
                Slice(key, sizeof(key)), 
                Slice(value, sizeof(value)));
        if(!s.ok())
        {
            printf("put error|%s\n", s.ToString().c_str());
            return 0;
        }
    }

    uint64_t end = NowMicros();
    
    printf("time: %u\n", end - begin);

    delete p_db;

    return 0;
}

int BenchmarkRead()
{
    DB *p_db = NULL;
    Options opt;
    Status s = DB::Open(opt, "./db_test", &p_db);

    if(NULL == p_db)
    {
        printf("open db error|%s\n", s.ToString().c_str());
        return 0;
    }

    uint64_t begin = NowMicros();

    std::map<std::string, std::string> map_all_kv;
    s = p_db->GetAll(map_all_kv);

    uint64_t end = NowMicros();
    
    printf("time: %u\n", end - begin);

    printf("%u|%s|%s\n", map_all_kv.size(), 
            map_all_kv.begin()->first.c_str(),
            (--map_all_kv.end())->first.c_str());

    delete p_db;
    
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("usage:%s read|write\n", argv[0]);
        return 0;
    }

    std::string bench_type(argv[1]);

    if(bench_type == "read")
    {
        BenchmarkRead();
    }
    else if(bench_type == "write")
    {
        BenchmarkWrite();
    }
    
    return 0;
}
