#ifndef EASYDB_DB_UTIL_H
#define EASYDB_DB_UTIL_H 

#include <ctime>
#include <cstdint>

namespace easydb 
{

time_t Now();

uint64_t NowMicros();

}

#endif
