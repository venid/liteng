
#include "data.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

const unsigned int DATA_BLOCK_SIZE = 128;

void Data :: realloc(unsigned int len)
 { if(len > (max_len - length + DATA_BLOCK_SIZE)) max_len += len;
   else max_len += DATA_BLOCK_SIZE;
   unsigned char *tmp  = new unsigned char [max_len];
   if(length > 0)
    { memcpy(tmp, bits, length);
      delete [] bits;
    }
   bits = tmp;
 }

template<>
bool Data :: get(std::string &vol)
 { if(bits == nullptr) return false;
   vol.clear();
   for(; pos < length && bits[pos] != '\n' && bits[pos] != '\0'; pos++)
    vol += bits[pos];
   pos++;
   return true;
 }

bool Data :: get(void *ptr, int len)
 { if(pos + len > length) return false;
   memcpy(ptr, bits + pos, len);
   pos += len;
   return true;
 }

template<>
void Data :: add(const std::string& ret)
 { unsigned int sLen = ret.length() + 1;
   if(sLen > (max_len - length)) realloc(sLen);
   memcpy(&bits[length], ret.c_str(), sLen);
   length += sLen;
 }

void Data :: add(const char* str)
 { unsigned int sLen = strlen(str) + 1;
   if(sLen > (max_len - length)) realloc(sLen);
   memcpy(&bits[length], str, sLen);
   length += sLen;
 }

void Data :: add(void *ptr, unsigned int len)
 { if(len > (max_len - length)) realloc(len);
   memcpy(&bits[length], ptr, len);
   length += len;
 }

bool Data :: setDataFromFile(const char *fileName, bool bit)
 { FILE *file = bit ? fopen(fileName, "wb") : fopen(fileName, "w");
   if(file == nullptr) return false;
   unsigned int n = fwrite(bits, 1, length, file);
   if(n != length)
    { fclose(file);
      return false;
    }
   fclose(file);
   return true;
 }

Data* Data :: getDataFromFile(const char *fileName)
 { struct stat fs;

   if(stat(fileName, &fs) == -1) return nullptr;
   unsigned int sz = fs.st_size;
   char *buf = new char [sz];
   if(buf == nullptr) return nullptr;
   FILE *file = fopen(fileName, "rb");
   if(file == nullptr)
    { delete [] buf;
      return nullptr;
    }
   unsigned int n = fread(buf, 1, sz, file);
   if(n != fs.st_size)
    { delete [] buf;
      fclose(file);
      return nullptr;
    }
   fclose(file);
   Data *data = new Data(buf, sz);
   return data;
 }
