#include <cstdio>
#include <string>

static std::string str_db_dir;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("usage:%s db_dir\n", argv[0]);
        return 0;
    }

    str_db_dir.assign(argv[1]);
    
    return 0;
}
