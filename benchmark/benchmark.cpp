#include <cstdio>
#include <sys/time.h>
#include "easydb/db.h"

using namespace easydb;

uint64_t NowMicros() 
{
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}


int main()
{
    DB *p_db = NULL;
    Options opt;
    //opt.max_log_file_size = 100;
    opt.file_sync_interval = 10;
    Status s = DB::Open(opt, "./db_test", &p_db);

    if(NULL == p_db)
    {
        printf("open db error|%s\n", s.ToString().c_str());
        return 0;
    }

    uint64_t begin = NowMicros();

    for(int i = 0; i < 10000000; ++i)
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
