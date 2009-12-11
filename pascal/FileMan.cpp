/*
 * FileMan utilities.
 *
 * L - list dir
 * E - 
 */


#include <cstdio>
#include <algorithm>
#include <memory>

#include "File.h"
#include "DateRec.h"
#include "../BlockDevice.h"


const char *MonthName(unsigned m)
{
    static const char *months[] = {
        "",
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };

    if (m > 12) return "";
    return months[m];
}

const char *FileType(unsigned ft)
{
    static const char *types[] = {
        "Unknown",
        "Badblocks",
        "Codefile",
        "Textfile",
        "Infofile",
        "Datafile",
        "Graffile",
        "Fotofile",
        "SecureDir"
    
    };
    
    if (ft < 8) return types[ft];
    
    return "";
}

void list(Pascal::VolumeEntry *volume, bool extended)
{
    unsigned fileCount = volume->fileCount();
    unsigned used = volume->blocks();
    unsigned max = 0;
    
    std::fprintf(stdout, "%s:\n", volume->name()); 
    
    for (unsigned i = 0; i < fileCount; ++i)
    {
        Pascal::FileEntry *e = volume->fileAtIndex(i);
        if (!e) continue;
        
        Pascal::DateRec dt = e->modification();
        
        //TODO -- include gaps.
        
        if (extended)
        {
        
             std::fprintf(stdout, "%-15s %4u %2u-%s-%2u %5u %5u  %s\n",
                e->name(), 
                e->blocks(),
                dt.day(),
                MonthName(dt.month()),
                dt.year() % 100,
                e->firstBlock(),
                e->lastByte(),
                FileType(e->fileKind())
            ); 
       
        }
        else
        {
            std::fprintf(stdout, "%-15s %4u %2u-%s-%2u\n",
                e->name(), 
                e->blocks(),
                dt.day(),
                MonthName(dt.month()),
                dt.year() % 100
            ); 
        }
        used += e->blocks();
        max = std::max(max, e->blocks());
    }
    // TODO -- largest refers to largest unused block,
    // not the largest file.
    
    std::fprintf(stdout, 
        "%u/%u files<listed/in-dir>, "
        "%u blocks used, "
        "%u unused, "
        "%u in largest\n",
        fileCount, fileCount,
        used,
        volume->volumeBlocks() - used,
        max
    );
    

}


int main(int argc, char **argv)
{
    std::auto_ptr<Pascal::VolumeEntry> volume;
    std::auto_ptr<ProFUSE::BlockDevice> device;
    
    
    const char *file = argv[1];
    
    try {
    
    device.reset( new ProFUSE::DOSOrderDiskImage(file, true));
    
    volume.reset( new Pascal::VolumeEntry(device.get()));
    
    device.release();

    list(volume.get(), true);
    
    }
    catch (ProFUSE::Exception& e)
    {
        std::fprintf(stderr, "%s\n", e.what());
        std::fprintf(stderr, "%s\n", strerror(e.error()));
    }
    

}