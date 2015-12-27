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
    fd_ = open(fname.c_str(), O_WRONLY|O_CREAT|O_APPEND|O_SYNC);
}

WritableFile::~WritableFile()
{
    if(fd_ > 0)
    {
        close(fd_);
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

}
