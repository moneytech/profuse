//
//  SDKImage.cpp
//  profuse
//
//  Created by Kelvin Sherlock on 3/6/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "SDKImage.h"

#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include <NufxLib.h>


#include <File/File.h>
#include <File/MappedFile.h>

#include <ProFUSE/Exception.h>

    
using ProFUSE::Exception;
using ProFUSE::POSIXException;


class NuFXException : public Exception
{
public:
    
    NuFXException(const char *cp, NuError error);
    NuFXException(const std::string& string, NuError error);
    
    virtual const char *errorString();

};


inline NuFXException::NuFXException(const char *cp, NuError error) :
Exception(cp, error)
{
}

inline NuFXException::NuFXException(const std::string& string, NuError error) :
Exception(string, error)
{
}

const char *NuFXException::errorString()
{
    return ::NuStrError((NuError)error());
}


using namespace Device;

struct record_thread
{
    NuRecordIdx record_index;
    NuThreadIdx thread_index;
};

/*
 * callback function to scan contents. 
 * (not used).
 *
 */
static NuResult ContentFunction(NuArchive *archive, void *vp)
{
    const NuRecord *record = (const NuRecord *)vp;
    
    /*
     * The application must not attempt to retain a copy of "pRecord" 
     * after the callback returns, as the structure may be freed.  
     * Anything of interest should be copied out.
     */
    
    
    for (unsigned i = 0; i < NuRecordGetNumThreads(record); ++i)
    {
        const NuThread *thread = NuGetThread(record, i);
        
        
        printf("%ld, %ld\n", (long)record->recordIdx, (long)thread->threadIdx);
        
        if (NuGetThreadID(thread) == kNuThreadIDDiskImage)
        {
            record_thread *rt;
            
            NuGetExtraData(archive, (void **)&rt);
            if (rt)
            {
                rt->record_index = record->recordIdx;
                rt->thread_index = thread->threadIdx;
            }
            
            return kNuAbort;
        }
        
    }
    
    
    return kNuOK;
}

static record_thread FindDiskImageThread(NuArchive *archive)
{
#undef __METHOD__
#define __METHOD__ "SDKImage::FindThread"
    
    record_thread rt;
    NuError e;
    NuAttr recordCount;
    
    e = NuGetAttr(archive, kNuAttrNumRecords, &recordCount);
    if (e)
    {
        throw NuFXException(__METHOD__ ": NuGetAttr", e);
    }
    
    for (unsigned position = 0; position < recordCount; ++position)
    {
        NuRecordIdx rIndex;
        const NuRecord *record;
        
        e = NuGetRecordIdxByPosition(archive, position, &rIndex);
        if (e)
        {
            throw NuFXException(__METHOD__ ": NuGetRecordIdxByPosition", e);
        }
        
        e = NuGetRecord(archive, rIndex, &record);
        if (e)
        {
            throw NuFXException(__METHOD__ ": NuGetRecord", e);
        }
    
        for (unsigned i = 0; i < NuRecordGetNumThreads(record); ++i)
        {
            const NuThread *thread = NuGetThread(record, i);
            
            if (NuGetThreadID(thread) == kNuThreadIDDiskImage)
            {
                rt.thread_index = thread->threadIdx;
                rt.record_index = record->recordIdx;
                return rt;
            }
        }   
    }
    
    throw Exception(__METHOD__ ": not a disk image");
}



/*
 * helper function to extract SDK image to /tmp and return a 
 * ProDOSDiskImage of the /tmp file.
 *
 */
BlockDevicePointer SDKImage::Open(const char *name)
{
#undef __METHOD__
#define __METHOD__ "SDKImage::Open"
    
    
    char tmp[] = "/tmp/pfuse.XXXXXXXX";
    
    int fd = -1;
    FILE *fp = NULL;
    NuArchive *archive = NULL;
    //const NuThread *thread = NULL;
    //const NuRecord *record = NULL;
    NuDataSink *sink = NULL;
    //NuRecordIdx rIndex;
    //NuThreadIdx tIndex;
    
    NuError e;


    record_thread rt = {0, 0};
    
    try {

    
        e = NuOpenRO(name, &archive);
        if (e)
        {
            throw NuFXException(__METHOD__ ": NuOpenRO", e);
        }

        rt = FindDiskImageThread(archive);
        
        fd = mkstemp(tmp);
        if (fd < 0)
        {
            throw POSIXException(__METHOD__ ": mkstemp", errno);
        }
        
        fp = fdopen(fd, "w");
        if (!fp)
        {
            ::close(fd);
            throw POSIXException(__METHOD__ ": fdopen", errno);            
        }
        
        e = NuCreateDataSinkForFP(true, kNuConvertOff, fp, &sink);
        if (e)
        {
            throw NuFXException(__METHOD__ ": NuCreateDataSinkForFP", e);
        }
        

        e = NuExtractThread(archive, rt.thread_index, sink);
        if (e)
        {
            throw NuFXException(__METHOD__ ": NuExtractThread", e);
        }
        
        fprintf(stderr, "Extracted disk image to %s\n", tmp);

        fclose(fp);
        NuClose(archive);
        NuFreeDataSink(sink);
        fp = NULL;
        archive = NULL;
        sink = NULL;
    }
    catch(...)
    {
        if (fp) fclose(fp);
        if (archive) NuClose(archive);
        if (sink) NuFreeDataSink(sink);
        
        throw;
    }

    // todo -- maybe SDKImage should extend ProDOSOrderDiskImage, have destructor
    // that unklinks the temp file.
    
    MappedFile file(tmp, File::ReadOnly);

    return ProDOSOrderDiskImage::Open(&file);

}