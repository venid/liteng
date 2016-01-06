
#pragma once

class Data;

enum FORMAT
 { FORMAT_NONE     = 0,

   // Unsigned formats
   FORMAT_R8       = 1,
   FORMAT_RG8      = 2,
   FORMAT_RGB8     = 3,
   FORMAT_RGBA8    = 4
 };



class Image
 { private:
    unsigned char *pixels;
    int width, height, depth;

    FORMAT format;

   public:
    Image();
    Image(const Image &img);
    ~Image();

    unsigned char* getPixels() const { return pixels; }
    int getWidth () const { return width;  }
    int getHeight() const { return height; }
    int getDepth () const { return depth;  }

    bool is1D()    const { return (depth == 0 && height == 0 && width > 0); }
    bool is2D()    const { return (depth == 0 && height >  0 && width > 0); }
    bool is3D()    const { return (depth >  1 && height >  0 && width > 0); }
    bool isCube()  const { return (depth == 1 && height >  0 && width > 0); }
    
    bool loadPNG(const Data* data);

 };
