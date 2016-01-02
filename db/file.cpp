#include "file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

namespace easydb
{

static Status IOError(const std::string& context, int err_number) 
{
    return Status::IOError(context, strerror(err_number));
}

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
#if defined(EASYDB_FILE_SYNC_ON_WRITE)
    fd_ = open(fname.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_SYNC, 0644);
#elif defined(EASYDB_FILE_SYNC_ON_DEMAND)
    fd_ = open(fname.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
#endif
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

uint64_t WritableFile::GetFileSize()
{
    uint64_t file_size = 0;
    easydb::GetFileSize(filename_, &file_size);
    return file_size;
}

const std::string &WritableFile::GetFileName()
{
    return filename_;
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

Status WritableFile::Sync()
{
    Status s;
    if(fdatasync(fd_) != 0)
    {
        s = Status::IOError(filename_, strerror(errno));
    }
    return s;
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

Status ReadDir(const std::string& dir,
        std::vector<std::string>* result) 
{
    result->clear();
    DIR* d = opendir(dir.c_str());
    if (d == NULL) {
        return Status::IOError(dir, strerror(errno));
    }   
    struct dirent* entry;
    while ((entry = readdir(d)) != NULL) {
        result->push_back(entry->d_name);
    }   
    closedir(d);
    return Status::OK();
}

Status DeleteFile(const std::string& fname) 
{
    Status result;
    if (unlink(fname.c_str()) != 0) {
        result = Status::IOError(fname, strerror(errno));
    }   
    return result;
}

Status GetFileSize(const std::string& fname, uint64_t* size) 
{
    Status s;
    struct stat sbuf;
    if (stat(fname.c_str(), &sbuf) != 0) {
        *size = 0;
        s = IOError(fname, errno);
    } else {
        *size = sbuf.st_size;
    }
    return s;
}

Status RenameFile(const std::string& src, const std::string& target) 
{
    Status result;
    if (rename(src.c_str(), target.c_str()) != 0) {
        result = IOError(src, errno);
    }
    return result;
}

int ParseFileName(const std::string& fname, FileType* type, std::vector<uint32_t> *vec_num)
{
    Slice rest(fname);
    *type = kUnknownFile;
    vec_num->clear();

    std::string str_num;
    while (!rest.empty()) 
    {
        char c = rest[0];
        if(c == '_')
        {
            vec_num->push_back(atoi(str_num.c_str()));
            str_num.clear();
        }
        else if(c == '.')
        {
            vec_num->push_back(atoi(str_num.c_str()));
            str_num.clear();
            break;
        }
        else
        {
            str_num.append(1, c);
        }
        rest.remove_prefix(1);
    }

    if(rest == Slice(".log"))
    {
        *type = kLogFile;
    }
    else if(rest == Slice(".mgi"))
    {
        *type = kMergingFile;
    }
    else if(rest == Slice(".mgd"))
    {
        *type = kMergedFile;
    }

    return 0;
}

}
