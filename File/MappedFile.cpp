#include "MappedFile.h"

#include <cerrno>
#include <sys/stat.h>

using namespace File;


MappedFile::MappedFile()
{
    _length = -1;
    _address = MAP_FAILED;
}

MappedFile::MappedFile(MappedFile &mf)
{
    _address = mf._address;
    _length = mf._length;
    
    mf._address = MAP_FAILED;
    mf._length = -1;
}

MappedFile::MappedFile(File f, int flags)
{
    struct stat st;
    
    // close enough
    if (f.fd() < 0)
        throw ProFUSE::PosixException(EBADF);


    if (::fstat(f.fd(), st) != 0)
        throw ProFUSE::PosixException(errno);
    
    if (!S_ISREG(st.st_mode))
        throw ProFUSE::PosixException(ENODEV);
        
    _length = st.st_size;
    _address = ::mmap(0, _length, 
        readOnly ? PROT_READ : PROT_READ | PROT_WRITE, 
        MAP_FILE | MAP_SHARED, f.fd(), 0); 

    if (_address == MAP_FAILED)
        throw ProFUSE::PosixException(errno);
}

MappedFile::~MappedFile()
{
    close();
}



MappedFile::close()
{
    if (_address != MAP_FAILED)
    {
        void *address = _address;
        size_t length = _length;
        
        _address = MAP_FAILED;
        _length = -1;
        
        if (::munmap(address, length) != 0)
            throw ProFUSE::PosixException(errno);
    }
}

MappedFile::sync()
{
    if (_address != MAP_FAILED)
    {
        if (::msync(_address, _length, MS_SYNC) != 0)
            throw ProFUSE::PosixException(errno);
    }
}