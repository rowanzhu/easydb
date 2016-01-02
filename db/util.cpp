#include <sys/time.h>
#include "util.h"

namespace easydb
{

time_t Now()
{
    return time(NULL);
}

uint64_t NowMicros() 
{
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

}
