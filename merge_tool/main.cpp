#include <cstdio>
#include "merger.h"
#include "db/util.h"

using namespace easydb;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("usage:%s db_dir\n", argv[0]);
        return 0;
    }

    Merger merger(argv[1]);
    
    time_t begin = Now();
    int ret = merger.Merge();
    time_t end = Now();

    if(0 != ret)
    {
        printf("merge failed|%d\n", ret);
    }
    else
    {
        printf("merge success|%u\n", end - begin);
    }
    
    return 0;
}
