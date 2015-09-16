
#pragma once

class Image
 { private:
    unsigned char *pixels;
    int width, height, depth;

   public:
    Image();
    Image(const Image &img);
    ~Image();

    int getWidth () const { return width;  }
    int getHeight() const { return height; }
    int getDepth () const { return depth;  }
 };
