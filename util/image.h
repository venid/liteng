
#pragma once

class Data;

class Image
 { private:
    unsigned char *pixels;
    int width, height, depth;

    int arrays;
    
   public:
    Image();
    Image(const Image &img);
    ~Image();

    int getWidth () const { return width;  }
    int getHeight() const { return height; }
    int getDepth () const { return depth;  }

    bool is1D()    const { return (depth == 0 && height == 0 && width > 0); }
    bool is2D()    const { return (depth == 0 && height >  0 && width > 0); }
    bool is3D()    const { return (depth >  1 && height >  0 && width > 0); }
    bool isCube()  const { return (depth == 1 && height >  0 && width > 0); }
    bool isArray() const { return (arrays > 1); }

    void loadPNG(const Data* data);

 };
