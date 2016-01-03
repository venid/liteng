
#include "image.h"
#include "log.h"

Image :: Image()
 { pixels = nullptr;
   width  = 0;
   height = 0;
   depth  = 0;
   arrays = 0;
 }

Image :: Image(const Image &img)
 { width  = img.width;
   height = img.height;
   depth  = img.depth;
   arrays = img.arrays;
 }

Image :: ~Image()
 { if(pixels) delete [] pixels;

 }

void Image :: loadPNG(const Data* data)
 {
 
 }
