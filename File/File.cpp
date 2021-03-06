#include <algorithm>
#include <cerrno>

#include <File/File.h>
#include <Common/Exception.h>
#include <POSIX/Exception.h>


File::File()
{
    _fd = -1;
}

File::File(int fd)
{
    _fd = fd;
}

File::File(File& f)
{
    _fd = f._fd;
    f._fd = -1;
}

File::File(const char *name, int flags, const std::nothrow_t&)
{
    _fd = ::open(name, flags);
}


File::File(const char *name, int flags, mode_t mode, const std::nothrow_t&)
{
    _fd = ::open(name, flags, mode);
}

File::File(const char *name, FileFlags flags, const std::nothrow_t&)
{
    _fd = ::open(name, flags == ReadOnly ? O_RDONLY : O_RDWR);
}

File::File(const char *name, int flags)
{
    #undef __METHOD__
    #define __METHOD__ "File::File"

    _fd = ::open(name, flags);
    if (_fd < 0)
        throw POSIX::Exception( __METHOD__ ": open", errno);
}

File::File(const char *name, int flags, mode_t mode)
{
#undef __METHOD__
#define __METHOD__ "File::File"
    
    _fd = ::open(name, flags, mode);
    if (_fd < 0)
        throw POSIX::Exception( __METHOD__ ": open", errno);
}


File::File(const char *name, FileFlags flags)
{
#undef __METHOD__
#define __METHOD__ "File::File"
    
    _fd = ::open(name, flags == ReadOnly ? O_RDONLY : O_RDWR);
    if (_fd < 0)
        throw POSIX::Exception( __METHOD__ ": open", errno);    
}


File::~File()
{
    close();
}

int File::release()
{
    int tmp = _fd;
    _fd = -1;
    return tmp;
}

void File::close()
{
    #undef __METHOD__
    #define __METHOD__ "File::close"

    if (_fd >= 0)
    {
        ::close(_fd);
        _fd = -1;
    
        // destructor shouldn't throw.
        /*
        if (::close(fd) != 0) 
            throw POSIX::Exception(__METHOD__ ": close", errno);
         */
    }
}


void File::adopt(File &f)
{
    if (&f == this) return;
    
    close();
    _fd = f._fd;
    f._fd = -1;
}

void File::adopt(int fd)
{
    if (fd == _fd) return;
    close();
    _fd = fd;
}


void File::swap(File &f)
{
    std::swap(_fd, f._fd);
}
