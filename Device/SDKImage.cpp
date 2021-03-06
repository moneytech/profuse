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
#include <cstring>

#include <NufxLib.h>


#include <File/File.h>
#include <File/MappedFile.h>

#include <Common/Exception.h>
#include <NuFX/Exception.h>
#include <POSIX/Exception.h>


using namespace Device;

struct record_thread
{
    NuRecordIdx record_index;
    NuThreadIdx thread_index;
};


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
        throw NuFX::Exception(__METHOD__ ": NuGetAttr", e);
    }
    
    for (unsigned position = 0; position < recordCount; ++position)
    {
        NuRecordIdx rIndex;
        const NuRecord *record;
        
        e = NuGetRecordIdxByPosition(archive, position, &rIndex);
        if (e)
        {
            throw NuFX::Exception(__METHOD__ ": NuGetRecordIdxByPosition", e);
        }
        
        e = NuGetRecord(archive, rIndex, &record);
        if (e)
        {
            throw NuFX::Exception(__METHOD__ ": NuGetRecord", e);
        }
    
        for (unsigned i = 0; i < NuRecordGetNumThreads(record); ++i)
        {
            const NuThread *thread = NuGetThread(record, i);
            
            if (thread && NuGetThreadID(thread) == kNuThreadIDDiskImage)
            {
                rt.thread_index = thread->threadIdx;
                rt.record_index = record->recordIdx;
                return rt;
            }
        }   
    }
    
    throw ::Exception(__METHOD__ ": not a disk image");
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
            throw NuFX::Exception(__METHOD__ ": NuOpenRO", e);
        }

        rt = FindDiskImageThread(archive);
        
        fd = mkstemp(tmp);
        if (fd < 0)
        {
            throw POSIX::Exception(__METHOD__ ": mkstemp", errno);
        }
        
        fp = fdopen(fd, "w");
        if (!fp)
        {
            ::close(fd);
            throw POSIX::Exception(__METHOD__ ": fdopen", errno);            
        }
        
        e = NuCreateDataSinkForFP(true, kNuConvertOff, fp, &sink);
        if (e)
        {
            throw NuFX::Exception(__METHOD__ ": NuCreateDataSinkForFP", e);
        }
        

        e = NuExtractThread(archive, rt.thread_index, sink);
        if (e)
        {
            throw NuFX::Exception(__METHOD__ ": NuExtractThread", e);
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



bool SDKImage::Validate(MappedFile * f, const std::nothrow_t &)
{

    // NuFile, alternating ASCII.
    static const char IdentityCheck[6] = { 0x4E, 0xF5, 0x46, 0xE9, 0x6C, 0xE5 };
    static const char BXYIdentityCheck[3] = { 0x0A, 0x47, 0x4C };
    
    uint8_t *address = (uint8_t *)f->address();
    size_t length = f->length();
    
    // check for a BXY header
    if (length >= 128 
        && std::memcmp(address, BXYIdentityCheck, sizeof(BXYIdentityCheck)) == 0)
    {
        length -= 128;
        address += 128;
    }
    
    
    if (length > sizeof(IdentityCheck)
        && std::memcmp(address, IdentityCheck, sizeof(IdentityCheck)) == 0)
        return true;
    
    
    return false;
    
}

bool SDKImage::Validate(MappedFile * f)
{
#undef __METHOD__
#define __METHOD__ "SDKImage::Validate"
    
    if (!Validate(f, std::nothrow))
        throw ::Exception(__METHOD__ ": Invalid file format.");

    return true;
}



