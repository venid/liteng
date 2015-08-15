/*
 17.04.2014
*/

#ifndef _DATA_H_
#define _DATA_H_

#include <memory.h>
#include <string>

class Data
 {
   private:
    unsigned char *bits;
    unsigned int length;
    unsigned int max_len;
    unsigned int pos;

    void realloc(unsigned int newLen);

   public:
    Data()
     { bits = nullptr;
       length = 0;
       max_len = 0;
       pos = 0;
     }

    Data(void *ptr, unsigned int len)
     { bits = (unsigned char *)ptr;
       length = len;
       max_len = len;
       pos = 0;
     }

    ~Data()
      { if(bits) delete [] bits; }

    bool isOk() const
     { return bits != nullptr; }

    void clear()
     { if(bits)
        { delete [] bits;
          bits = nullptr;
          length = 0;
          max_len = 0;
        }
     }

    int getLength() const
     { return length; }

    void* getPtr() const
     { return bits + pos; }

    void* getPtr(unsigned int offs) const
     { if(offs >= length) return nullptr;
       return bits + offs;
     }

    int seekCur(unsigned int delta)
     { pos += delta;
       if(pos > length) pos = length;
       if(pos < 0) pos = 0;
       return pos;
     }

    int seekAbs(int offs)
     { pos = offs;
       if(pos > length) pos = length;
       if(pos < 0) pos = 0;
       return pos;
     }

    template<typename T>
    bool get(T& vol)
     { int sz = sizeof(T);
       if(pos + sz > length) return false;
       vol = *(T*) (bits + pos);
       pos += sz;
       return true;
     }
    bool get(void* ptr, int len);

    template<typename T>
    void add(const T& ret)
     { if(sizeof(T) > (max_len - length)) realloc(sizeof(T));
       memcpy(&bits[length], &ret, sizeof(T));
       length += sizeof(T);
     }
    void add(const char* str);
    void add(void *ptr, unsigned int len);

    bool setDataFromFile(const char *fileName, bool bit = true);

    static Data* getDataFromFile(const char *fileName);
 };

#endif // _DATA_H_
