
#pragma once

#include <string>
#include <list>
#include "resource.h"

class Data;

#pragma pack (push, 1)

struct ZipLocalFileHeader
 { unsigned int   signature;
   unsigned short versionNeededToExtract;
   unsigned short generalPurposeBitFlag;
   unsigned short compressionMethod;
   unsigned short lastModFileTime;
   unsigned short lastModFileDate;
   unsigned int   crc32;
   int            compressedSize;
   int            uncompressedSize;
   unsigned short filenameLength;
   unsigned short extraFieldLength;
 };

struct ZipDataDescriptor
 { unsigned int crc32;
   unsigned int compressedSize;
   unsigned int uncompressedSize;
 };

#pragma pack (pop)

class ZipFileSystem : public Resource
 {
   private:
    struct ZipHeader
     { ZipLocalFileHeader hdr;
       size_t offset;
     };

    typedef std::list <std::pair<std::string, ZipHeader> > ZipDir;
    std::string fileName;
    ZipDir dir;          // contains directory of zip file
    bool broken;

    void readDirectory( int file );
    Data  * readEntry(const ZipLocalFileHeader& hdr, size_t offset);

   public:
    ZipFileSystem(const std::string& fName);

    Data* getFile(const std::string& theName);
 };
