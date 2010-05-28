/*
 * FileMan utilities.
 *
 * L - list dir
 * E - 
 */


#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <algorithm>
#include <memory>

#include <unistd.h>
#include <strings.h>
#include <sys/stat.h>

#include <Pascal/Pascal.h>
#include <Pascal/Date.h>
#include <Device/BlockDevice.h>

#include <File/File.h>

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

void printUnusedEntry(unsigned block, unsigned size)
{
    std::printf("< UNUSED >      %4u            %4u\n", size, block);
}

void printFileEntry(Pascal::FileEntry *e, bool extended)
{
    Pascal::Date dt = e->modification();

    if (extended)
    {
         std::printf("%-15s %4u %2u-%s-%2u %5u %5u  %s\n",
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
        std::printf("%-15s %4u %2u-%s-%2u\n",
            e->name(), 
            e->blocks(),
            dt.day(),
            MonthName(dt.month()),
            dt.year() % 100
        ); 
    }

}

int action_ls(int argc, char **argv, Pascal::VolumeEntry *volume)
{    
    bool extended = false;
    unsigned fileCount = volume->fileCount();
    unsigned used = volume->blocks();
    unsigned max = 0;
    unsigned volumeSize = volume->volumeBlocks();
    unsigned lastBlock = volume->lastBlock();
    int ch;
    
    std::fprintf(stdout, "%s:\n", volume->name()); 
    
    //argv[0] = "afpm ls";
    
    while ((ch = ::getopt(argc, argv, "l")) != -1)
    {
        switch(ch)
        {
        case 'l':
            extended = true;
            break;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    for (unsigned i = 0; i < fileCount; ++i)
    {
        Pascal::FileEntry *e = volume->fileAtIndex(i);
        if (!e) continue;
        
        
        if (lastBlock != e->firstBlock())
        {
            unsigned size = e->firstBlock() - lastBlock;
            max = std::max(max, size);
        
            if (extended)
            {
                printUnusedEntry(lastBlock, size);
            }
        }
        
        printFileEntry(e, extended);
        
        lastBlock = e->lastBlock();
        used += e->blocks();
    }

    if (lastBlock != volumeSize)
    {
        unsigned size = volumeSize - lastBlock;
        max = std::max(max, size);    
        if (extended)
            printUnusedEntry(lastBlock, size);
    }    
    
    
    std::fprintf(stdout, 
        "%u/%u files <listed/in-dir>, "
        "%u blocks used, "
        "%u unused, "
        "%u in largest\n",
        fileCount, fileCount,
        used,
        volumeSize - used,
        max
    );
 
    return 0;
}


int action_cat(unsigned argc, char **argv, Pascal::VolumeEntry *volume)
{
    // cat file1, file2...
    //argv[0] = "afpm cat";
    
    if (argc < 2)
    {
        std::fprintf(stderr, "apfm cat: Please specify one or more files.\n");
        return 1;
    }

    for (unsigned i = 1; i < argc; ++i)
    {
        const char *fname = argv[i];
        unsigned fileSize;
        unsigned offset;
        uint8_t buffer[512];
        Pascal::FileEntry *e = NULL;
        // find it...

        for (unsigned i = 0, l = volume->fileCount(); i < l; ++i)
        {
            e = volume->fileAtIndex(i);
            if (::strcasecmp(e->name(), fname) == 0) break;
            e = NULL;
        }
        
        if (!e)
        {
            std::fprintf(stderr, "apfm cat: %s: no such file.\n", fname);
            continue;
        }
    
        fileSize = e->fileSize();
        offset = 0;
        while (offset < fileSize)
        {
            unsigned count = std::min(512u, fileSize - offset);
            e->read(buffer, count, offset);
            
            std::fwrite(buffer, count, 1, stdout);
            offset += count; 
        }
    }
    
    return 0;
}

int action_cp(int argc, char **argv, Pascal::VolumeEntry *volume)
{
    // cp src dest
    // first character of ':' indicates pascal file, otherwise, is native file?
    return 0;
}

int action_mv(int argc, char **argv, Pascal::VolumeEntry *volume)
{
    // mv src dest
    // first character of ':' indicates pascal file, otherwise is native file?
    return 0;
}

int action_rm(int argc, char **argv, Pascal::VolumeEntry *volume)
{
    // rm file [file ....]

    // TODO -- -f flag to prompt for each file.
    // TODO -- catch errors.
    for (unsigned i = 1; i < argc; ++i)
    {
        volume->unlink(argv[i]);
    }
    return 0;
    
}

int action_krunch(int argc, char **argv, Pascal::VolumeEntry *volume)
{
    // compress file to remove gaps.
    return 0;
}



int action_get(int argc, char **argv, Pascal::VolumeEntry *volume)
{
    // get pascal_file [native file];
    
    char *infile;
    char *outfile;
    
    Pascal::FileEntry *entry;
    
    switch(argc)
    {
        case 1:
            infile = outfile = argv[0];
            break;
        case 2:
            infile = argv[0];
            outfile = argv[1];
            break;
        default:
            std::fprintf(stderr, "apfm cat: Please specify an infile (and an optional outfile)\n");
    }
    
    entry = volume->fileByName(infile);
    
    
    File::File file(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    
    if (!entry)
    {
        std::fprintf(stderr, "apfm get: %s: no such file.\n", infile);
        return 1;
    }
    
    unsigned fileSize = entry->fileSize();
    unsigned offset = 0;
    
    while (offset < fileSize)
    {
        uint8_t buffer[512];
        
        unsigned count = std::min(512u, fileSize - offset);
        entry->read(buffer, count, offset);
        
        ::write(file.fd(), buffer, count);
        offset += count; 
    }
    
    return 0;
}

#if 0
int action_put(int argc, char **argv, Pascal::VolumeEntry *volume)
{
    // put [-t type] native_file [pascal_file]
    
    unsigned type = Pascal::kUntypedFile;
    
    struct stat st;
    int c;
    
    char *infile;
    char *outfile;
    char *tmp;
    
    while ((c = getopt(argc, argv, "t:")) != -1)
    {
        switch (c)
        {
            case 't':
                if (!::strcasecmp("text", optarg))
                    type = Pascal::kTextFile;
                else if (!::strcasecmp("txt", optarg))
                    type = Pascal::kTextFile;
                else if (!::strcasecmp("code", optarg))
                    type = Pascal::kCodeFile;
                else if (!::strcasecmp("info", optarg))
                    type = Pascal::kInfoFile;
                else if (!::strcasecmp("data", optarg))
                    type = Pascal::kDataFile;
                else if (!::strcasecmp("graf", optarg))
                    type = Pascal::kGrafFile;
                else if (!::strcasecmp("foto", optarg))
                    type = Pascal::kFotoFile;
                else type = Pascal::kUntypedFile;
                    
                break;
        }
        
    }
    
    // TODO -- if file is named .txt or .text, default to kTextFile.
    
    argc -= optind;
    argv += optind;
    
    switch (argc)
    {
        case 1:
            infile = outfile = argv[0];
            
            // need to run basename on outfile.
            tmp = strrchr(outfile, '/');
            if (tmp) outfile = tmp + 1;
            break;
        case 2:
            infile = argv[0];
            outfile = argv[1];
            break;
    }
    
    if (!Pascal::FileEntry::ValidName(outfile))
    {
        std::fprintf(stderr, "apfm put: `%s' is not a valid pascal name.\n", outfile);
    }

    ::stat(infile, &st);

    File::File file(infile, O_RDONLY);
    
    
    unsigned blocks = (st.st_size + 511) / 511;
    
    // TODO -- if text file, ..
    
    Pascal::FileEntry *entry = volume.createFile(infile, blocks);
    
    entry->setType(type);
    
    if (type == Pascal::kTextFile)
    {
        //...
    }
    else
    {
        // ...
        
    }

    return 0;
    
}
#endif

void usage()
{
    std::printf(
        "Pascal File Manager v 0.0\n\n"
        "Usage: fileman [-h] [-f format] diskimage action ...\n"
        "Options:\n"
        "  -h            Show usage information.\n"
        "  -f format     Specify disk format.  Valid values are:\n"
        "                  po: ProDOS order disk image\n"
        "                  do: DOS Order disk image\n"
        "\n"
        "Actions:\n"
        "  cat\n"
        "  cp\n"
        "  krunch\n"
        "  ls\n"
        "  mv\n"
        "  rm\n"              
    );

}

int main(int argc, char **argv)
{
    std::auto_ptr<Pascal::VolumeEntry> volume;
    std::auto_ptr<Device::BlockDevice> device;
  
    unsigned fmt = 0;
    
    int c;
    

    #ifdef __linux__
    putenv((char *)"POSIXLY_CORRECT=1"); // fix getopt to not mutate
    #endif

    /*
    char *argv2[] = {
        (char *)"afpm",
        //(char *)"/Users/kelvin/Desktop/ucsd/pascal.dsk",
        (char *)"/Users/kelvin/Desktop/ucsd/UCSD Pascal 1.2_3.DSK",
        (char *)"cat",
        (char *)"SPIRODEMO.TEXT",
        NULL
    };
    char **argv = argv2;
    int argc = 4;
    */

    // getop stops at first non '-' arg so it will not affect action flags.    
    while ((c = ::getopt(argc, argv, "f:h")) != -1)
    {
        std::printf("%c\n", c);
        switch(c)
        {
        case 'f':
            fmt = Device::BlockDevice::ImageType(optarg);
            if (!fmt)
            {
                std::fprintf(stderr, "Error: Invalid file format: ``%s''.\n",
                    optarg);
            }
            break;
            
        case 'h':
        case '?':
        case ':':
            usage();
            return c == 'h' ? 0 : 1;
        }
    }
    

    argc -= optind;
    argv += optind;

    #ifdef __linux__
    optind = 0;
    #else
    optreset = 1;
    optind = 1;
    #endif
    
    if (argc < 2)
    {
        usage();
        return 0;
    }
    
    
    const char *file = argv[0];
    const char *action = argv[1];
    
        
    
    try {
        
        // should we peek at the action to determine if read only?
        
        device.reset( Device::BlockDevice::Open(file, false, fmt) );
    

                     
                     
        
        volume.reset( new Pascal::VolumeEntry(device.get()));
        
        device.release();


        if (!::strcasecmp("cat", action)) return action_cat(argc - 1, argv + 1, volume.get());
        if (!::strcasecmp("cp", action)) return action_cp(argc - 1, argv + 1, volume.get());
        if (!::strcasecmp("krunch", action)) return action_krunch(argc - 1, argv + 1, volume.get());
        if (!::strcasecmp("ls", action)) return action_ls(argc - 1, argv + 1, volume.get());
        if (!::strcasecmp("mv", action)) return action_mv(argc - 1, argv + 1, volume.get());
        if (!::strcasecmp("rm", action)) return action_rm(argc - 1, argv + 1, volume.get());

        if (!::strcasecmp("get", action)) return action_get(argc -1, argv + 1, volume.get());
        //if (!::strcasecmp("put", action)) return action_put(argc -1, argv + 1, volume.get());
        
        
        usage();
        return 3;
    }
    catch (ProFUSE::Exception& e)
    {
        std::fprintf(stderr, "%s\n", e.what());
        std::fprintf(stderr, "%s\n", strerror(e.error()));
    }
    
    return 0;
}
