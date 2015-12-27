#include <cstdio>
#include "easydb/db.h"

using namespace easydb;

int main()
{
    DB *p_db = NULL;
    Status s = DB::Open(Options(), "./db_test", &p_db);

    if(NULL == p_db)
    {
        printf("open db error|%s\n", s.ToString().c_str());
        return 0;
    }

    s = p_db->Put(WriteOptions(), "hello", "world");
    if(!s.ok())
    {
        printf("put error|%s\n", s.ToString().c_str());
        return 0;
    }

    delete p_db;

    return 0;
}
