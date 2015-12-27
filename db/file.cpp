#include "file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace easydb
{

SequentialFile::SequentialFile(const std::string& fname)
    :filename_(fname)
{
    FILE* f = fopen(fname.c_str(), "r");
    if (f != NULL) 
    {
        file_ = f;
    }
}

SequentialFile::~SequentialFile()
{
    if(NULL != file_)
    {
        fclose(file_);
        file_ = NULL;
    }
}

bool SequentialFile::IsValid()
{
    if(NULL == file_)
    {
        return false;
    }else
    {
        return true;
    }
}

Status SequentialFile::Read(size_t n, Slice* result, char* scratch)
{
    if(NULL == file_)
    {
        return Status::IOError(filename_, "invalid file stream");
    }
    
    Status s;
    size_t r = fread_unlocked(scratch, 1, n, file_);
    *result = Slice(scratch, r);
    if (r < n) {
        if (feof(file_)) {
            // We leave status as ok if we hit the end of the file
        } else {
            // A partial read with an error: return a non-ok status
            s = Status::IOError(filename_, strerror(errno));
        }
    }
    return s;
}

Status SequentialFile::Skip(uint64_t n)
{
    if(NULL == file_)
    {
        return Status::IOError(filename_, "invalid file stream");
    }
    
    if (fseek(file_, n, SEEK_CUR)) {
        return Status::IOError(filename_, strerror(errno));
    }
    return Status::OK();
}



WritableFile::WritableFile(const std::string& fname)
    :filename_(fname), fd_(-1)
{
    fd_ = open(fname.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_SYNC, 0644);
}

WritableFile::~WritableFile()
{
    if(fd_ > 0)
    {
        close(fd_);
    }
}

bool WritableFile::IsValid()
{
    if(fd_ < 0)
    {
        return false;
    }else
    {
        return true;
    }
}

Status WritableFile::Append(const Slice& data)
{
    if(fd_ < 0)
    {
        return Status::IOError(filename_, "invalid fd");
    }

    ssize_t ret = write(fd_, data.data(), data.size());
    if(ret != data.size())
    {
        return Status::IOError(filename_, strerror(errno));
    }
    return Status::OK();
}



Logger::Logger(const std::string& fname)
{

}

Logger::~Logger()
{

}

void Logger::Logv(const char* format, va_list ap)
{

}


Status CreateDir(const std::string& name) 
{
    Status result;
    int ret = mkdir(name.c_str(), 0755);
    if (ret != 0) {
        result = Status::IOError(name, strerror(errno));
    }   
    return result;
}

bool FileExists(const std::string& fname) 
{
    return access(fname.c_str(), F_OK) == 0;
}

}
