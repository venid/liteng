
#include "image.h"
#include "log.h"

Image :: Image()
 { pixels = nullptr;
   width  = 0;
   height = 0;
   depth  = 0;
 }

Image :: Image(const Image &img)
 { width  = img.width;
   height = img.height;
   depth  = img.depth;
 }

Image :: ~Image()
 { if(pixels) delete [] pixels;

 }
