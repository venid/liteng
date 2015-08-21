
#include "zipfilesystem.h"
#include "log.h"
#include "data.h"
#include <zlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <ctype.h>


#define O_BINARY 0

#define LOCAL_ZIP_SIGNATURE   0x04034B50
#define CENTRAL_ZIP_SIGNATURE 0x02014B50
#define END_CENTRAL_SIGNATURE 0x06054B50
#define EXTD_LOCAL_SIGNATURE  0x08064B50

#define DATA_ZIP_DESCRIPTOR 0x0004

#define DEF_WBITS 15   // Default LZ77 window size
#define ZIP_STORE 0    // 'STORED' method id
#define ZIP_DEFLATE 8

#define CHUNK_SIZE 1024 // chunk size for reading zip file from end

ZipFileSystem :: ZipFileSystem(const std::string& fName) : Resource(), fileName(fName)
 { int file = open(fileName.c_str(), O_BINARY | O_RDONLY);

   if(file == -1)
    { LOG_WARNING("ZipFileSystem: File \"%s\" not detected.", fileName.c_str());
      return;
    }
   broken = false;
   readDirectory(file);
   close(file);
   LOG_INFO("ZipFileSystem: Created resource source \"%s\" (files %i)", fileName.c_str(), dir.size());
 }

Data * ZipFileSystem :: getFile(const std::string& theName)
 { for( auto it : dir)
    if(it.first == theName) // found name
     return readEntry(it.second.hdr, it.second.offset);

   return nullptr;
 }

void ZipFileSystem :: readDirectory ( int file )
 { ZipLocalFileHeader hdr;
   size_t pos = 0;
   unsigned int sig;
   char buf[512];

   lseek ( file, 0, SEEK_SET );

   read(file, &sig, sizeof(unsigned int));

   while(sig == LOCAL_ZIP_SIGNATURE)
    { lseek ( file, pos, SEEK_SET );
      pos += read(file, &hdr, sizeof(hdr));

      if(hdr.compressedSize == 0)
       pos += hdr.filenameLength + hdr.extraFieldLength;
      else
       { pos += read(file, buf, hdr.filenameLength);
         buf[hdr.filenameLength] = '\0';
         pos += hdr.extraFieldLength;

         dir.push_front(std::make_pair(std::string(buf), ZipHeader{hdr, pos}));

         pos += hdr.compressedSize;
       }

      if((hdr.generalPurposeBitFlag & DATA_ZIP_DESCRIPTOR) == DATA_ZIP_DESCRIPTOR)
       pos += sizeof(ZipDataDescriptor);

      lseek ( file, pos, SEEK_SET );
      read(file, &sig, sizeof(unsigned int));
    }
 }

Data * ZipFileSystem :: readEntry (const ZipLocalFileHeader& hdr, size_t offset)
 { int size = 0;
   int file = open(fileName.c_str(), O_BINARY | O_RDONLY);

   if(file == -1) return nullptr;

   lseek(file, offset, SEEK_SET);

   char inBuffer[2048];
   size_t blockSize;
   size_t bytesLeft = hdr.compressedSize;
   void*  buf = calloc(1, hdr.uncompressedSize + 1);
   int    err = 0;

   switch ( hdr.compressionMethod )
    { case ZIP_STORE:
       if ( read ( file, buf, hdr.compressedSize ) != hdr.compressedSize )
        { free  ( buf );
          close ( file );
          return nullptr;
        }
       break;
      case ZIP_DEFLATE:
       z_stream zs;
       memset(&zs, '\0', sizeof(zs));
       zs.next_out  = (unsigned char *)buf;
       zs.avail_out = hdr.uncompressedSize;
       zs.zalloc    = nullptr;
       zs.zfree     = nullptr;

       if(inflateInit2(&zs, -DEF_WBITS) != Z_OK)
        { free  ( buf  );
          close ( file );
          return nullptr;
        }

       while ( bytesLeft > 0 )
        { blockSize   = bytesLeft > sizeof ( inBuffer ) ? sizeof ( inBuffer ) : bytesLeft;
          zs.next_in  = (unsigned char *)inBuffer;
          zs.avail_in = read ( file, inBuffer, blockSize );

          err = inflate( &zs, bytesLeft >= blockSize ? Z_PARTIAL_FLUSH : Z_FINISH );
          bytesLeft -= blockSize;
        }

       inflateEnd ( &zs );

       if(err < 0)
        { free  ( buf  );
          close ( file );
          return nullptr;
        }
       break;
      default:
       free  ( buf );
       close ( file );
       return nullptr;
    }

   close ( file );
   size = hdr.uncompressedSize;
   return new Data(buf, size);
 }
