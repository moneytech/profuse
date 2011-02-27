CC = g++
CPPFLAGS += -Wall -W -Wno-multichar -I. -O2 -g 
LDFLAGS += -lpthread
UNAME = $(shell uname)

ifeq ($(UNAME),Darwin)
    fuse_pascal_LDFLAGS += -lfuse_ino64
else
    fuse_pascal_LDFLAGS += -lfuse
endif





OBJECTS += ${wildcard *.o}
OBJECTS += ${wildcard bin/*.o}
OBJECTS += ${wildcard Cache/*.o}
OBJECTS += ${wildcard Device/*.o}
OBJECTS += ${wildcard Endian/*.o}
OBJECTS += ${wildcard File/*.o}
OBJECTS += ${wildcard Pascal/*.o}
OBJECTS += ${wildcard ProFUSE/*.o}

TARGETS = apfm newfs_pascal profuse_pascal xattr

BIN_OBJECTS += bin/apfm.o
BIN_OBJECTS += bin/fuse_pascal_ops.o
BIN_OBJECTS += bin/newfs_prodos.o
BIN_OBJECTS += bin/fuse_pascal.o
BIN_OBJECTS += bin/newfs_pascal.o
BIN_OBJECTS += bin/xattr.o

CACHE_OBJECTS += Cache/BlockCache.o
CACHE_OBJECTS += Cache/ConcreteBlockCache.o
CACHE_OBJECTS += Cache/MappedBlockCache.o

DEVICE_OBJECTS += Device/Adaptor.o
DEVICE_OBJECTS += Device/BlockDevice.o
DEVICE_OBJECTS += Device/DavexDiskImage.o
DEVICE_OBJECTS += Device/DiskCopy42Image.o
DEVICE_OBJECTS += Device/DiskImage.o
DEVICE_OBJECTS += Device/RawDevice.o
DEVICE_OBJECTS += Device/UniversalDiskImage.o

ENDIAN_OBJECTS += Endian/Endian.o

FILE_OBJECTS += File/File.o
FILE_OBJECTS += File/MappedFile.o

PASCAL_OBJECTS += Pascal/Date.o
PASCAL_OBJECTS += Pascal/FileEntry.o
PASCAL_OBJECTS += Pascal/TextWriter.o
PASCAL_OBJECTS += Pascal/Entry.o
PASCAL_OBJECTS += Pascal/VolumeEntry.o

PROFUSE_OBJECTS += ProFUSE/Exception.o
PROFUSE_OBJECTS += ProFUSE/Lock.o



xattr: bin/xattr.o
	$(CC) $(LDFLAGS) $^ -o $@

newfs_pascal: bin/newfs_pascal.o \
  ${CACHE_OBJECTS} \
  ${DEVICE_OBJECTS} \
  ${ENDIAN_OBJECTS} \
  ${FILE_OBJECTS} \
  ${PROFUSE_OBJECTS} \
  ${PASCAL_OBJECTS}
	$(CC) $(LDFLAGS) $^ -o $@

apfm: bin/apfm.o \
  ${CACHE_OBJECTS} \
  ${DEVICE_OBJECTS} \
  ${ENDIAN_OBJECTS} \
  ${FILE_OBJECTS} \
  ${PROFUSE_OBJECTS} \
  ${PASCAL_OBJECTS}
	$(CC) $(LDFLAGS) $^ -o $@


fuse_pascal: bin/fuse_pascal.o bin/fuse_pascal_ops.o \
  ${CACHE_OBJECTS} \
  ${DEVICE_OBJECTS} \
  ${ENDIAN_OBJECTS} \
  ${FILE_OBJECTS} \
  ${PROFUSE_OBJECTS} \
  ${PASCAL_OBJECTS}
	$(CC) $(fuse_pascal_LDFLAGS) $(LDFLAGS) $^ -o $@


clean:
	rm -f  ${OBJECTS} ${TARGETS}

xattr.o: bin/xattr.cpp
 
newfs_pascal.o: bin/newfs_pascal.cpp Device/BlockDevice.h ProFUSE/Exception.h \
  Device/TrackSector.h Cache/BlockCache.h Device/RawDevice.h File/File.h \
  Pascal/Pascal.h Pascal/Date.h

fuse_pascal.o: bin/fuse_pascal.cpp Pascal/Pascal.h Pascal/Date.h \
  ProFUSE/Exception.h Device/BlockDevice.h Device/TrackSector.h \
  Cache/BlockCache.h

fuse_pascal_ops.o: bin/fuse_pascal_ops.cpp Pascal/Pascal.h Pascal/Date.h \
  ProFUSE/auto.h ProFUSE/Exception.h

apfm.o: bin/apfm.cpp Pascal/Pascal.h Pascal/Date.h Device/BlockDevice.h \
  ProFUSE/Exception.h Device/TrackSector.h Cache/BlockCache.h

File/File.o: File/File.cpp File/File.h ProFUSE/Exception.h

File/MappedFile.o: File/MappedFile.cpp File/MappedFile.h File/File.h \
  ProFUSE/Exception.h

Device/Adaptor.o: Device/Adaptor.cpp Device/Adaptor.h Device/TrackSector.h \
  ProFUSE/Exception.h

Device/BlockDevice.o: Device/BlockDevice.cpp Device/BlockDevice.h \
  ProFUSE/Exception.h Device/TrackSector.h Cache/BlockCache.h \
  Cache/ConcreteBlockCache.h Device/DiskImage.h Device/Adaptor.h \
  File/MappedFile.h File/File.h Device/UniversalDiskImage.h \
  Device/DiskCopy42Image.h Device/DavexDiskImage.h Device/RawDevice.h

Device/DavexDiskImage.o: Device/DavexDiskImage.cpp \
  Device/DavexDiskImage.h \
  Device/BlockDevice.h ProFUSE/Exception.h Device/TrackSector.h \
  Cache/BlockCache.h Device/DiskImage.h Device/Adaptor.h \
  File/MappedFile.h File/File.h Endian/Endian.h Endian/IOBuffer.h \
  Endian/IOBuffer.cpp.h Cache/MappedBlockCache.h

Device/DiskCopy42Image.o: Device/DiskCopy42Image.cpp \
  Device/DiskCopy42Image.h \
  Device/BlockDevice.h ProFUSE/Exception.h Device/TrackSector.h \
  Cache/BlockCache.h Device/DiskImage.h Device/Adaptor.h \
  File/MappedFile.h File/File.h Endian/Endian.h Endian/IOBuffer.h \
  Endian/IOBuffer.cpp.h Cache/MappedBlockCache.h

Device/DiskImage.o: Device/DiskImage.cpp Device/DiskImage.h \
  ProFUSE/Exception.h \
  Device/BlockDevice.h Device/TrackSector.h Cache/BlockCache.h \
  Device/Adaptor.h File/MappedFile.h File/File.h Cache/MappedBlockCache.h

Device/RawDevice.o: Device/RawDevice.cpp Device/RawDevice.h \
  Device/BlockDevice.h \
  ProFUSE/Exception.h Device/TrackSector.h Cache/BlockCache.h File/File.h

Device/UniversalDiskImage.o: Device/UniversalDiskImage.cpp \
  Device/UniversalDiskImage.h Device/BlockDevice.h ProFUSE/Exception.h \
  Device/TrackSector.h Cache/BlockCache.h Device/DiskImage.h \
  Device/Adaptor.h File/MappedFile.h File/File.h Endian/Endian.h \
  Endian/IOBuffer.h Endian/IOBuffer.cpp.h Cache/MappedBlockCache.h \
  Cache/ConcreteBlockCache.h

Endian/Endian.o: Endian/Endian.cpp Endian/Endian.h

Cache/BlockCache.o: Cache/BlockCache.cpp Cache/BlockCache.h \
  Device/BlockDevice.h ProFUSE/Exception.h Device/TrackSector.h \
  ProFUSE/auto.h

Cache/ConcreteBlockCache.o: Cache/ConcreteBlockCache.cpp \
  Device/BlockDevice.h \
  ProFUSE/Exception.h Device/TrackSector.h Cache/BlockCache.h \
  Cache/ConcreteBlockCache.h ProFUSE/auto.h

Cache/MappedBlockCache.o: Cache/MappedBlockCache.cpp \
  Cache/MappedBlockCache.h \
  Cache/BlockCache.h Device/BlockDevice.h ProFUSE/Exception.h \
  Device/TrackSector.h

ProFUSE/Exception.o: ProFUSE/Exception.cpp ProFUSE/Exception.h

ProFUSE/Lock.o: ProFUSE/Lock.cpp ProFUSE/Lock.h


Pascal/Date.o: Pascal/Date.cpp Pascal/Date.h

Pascal/Entry.o: Pascal/Entry.cpp Pascal/Entry.h Pascal/Date.h \
  ProFUSE/Exception.h Endian/Endian.h Endian/IOBuffer.h \
  Endian/IOBuffer.cpp.h Device/BlockDevice.h Device/TrackSector.h \
  Cache/BlockCache.h

Pascal/FileEntry.o: Pascal/FileEntry.cpp Pascal/Pascal.h Pascal/Date.h \
  Pascal/Entry.h Pascal/FileEntry.h Pascal/VolumeEntry.h ProFUSE/auto.h \
  ProFUSE/Exception.h Endian/Endian.h Endian/IOBuffer.h \
  Endian/IOBuffer.cpp.h Device/BlockDevice.h Device/TrackSector.h \
  Cache/BlockCache.h Pascal/TextWriter.h

Pascal/VolumeEntry.o: Pascal/VolumeEntry.cpp Pascal/Pascal.h Pascal/Date.h \
  Pascal/Entry.h Pascal/FileEntry.h Pascal/VolumeEntry.h ProFUSE/auto.h \
  ProFUSE/Exception.h Endian/Endian.h Endian/IOBuffer.h \
  Endian/IOBuffer.cpp.h Device/BlockDevice.h Device/TrackSector.h \
  Cache/BlockCache.h

Pascal/TextWriter.o: Pascal/TextWriter.cpp Pascal/TextWriter.h \
  Pascal/FileEntry.h Pascal/Entry.h Pascal/Date.h ProFUSE/Exception.h
