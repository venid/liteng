
#include "image.h"
#include "log.h"
#include "data.h"
#include <libpng/png.h>
#include <zlib.h>

Image :: Image()
 { pixels = nullptr;
   width  = 0;
   height = 0;
   depth  = 0;
   format = FORMAT_NONE;
 }

Image :: Image(const Image &img)
 { width  = img.width;
   height = img.height;
   depth  = img.depth;
   format = img.format;
 }

Image :: ~Image()
 { if(pixels) delete [] pixels; }

/// -------------------------------------PNG-------------------------------------------------

struct PngImageRawData
 { unsigned char *data;
   size_t size;
 };

static void pngImageRead ( png_structp pngPtr, png_bytep data, png_size_t size )
 { PngImageRawData * self = (PngImageRawData *) pngPtr -> io_ptr;

   if(self->size < size) png_error(pngPtr, "Read error");
   else
    { memcpy ( data, self->data, size );
      self -> size -= size;
      self -> data += size;
    }
 }

// -------------------------------------------------------------------------------------------

bool Image :: loadPNG(const Data* data)
 { if(!png_check_sig((unsigned char*)data->getPtr(), data->getLength()))
    { LOG_WARNING("Texture: Unknown format(not PNG)"); return false; }

   png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
   if(pngPtr == nullptr) return false;

   png_infop infoPtr = png_create_info_struct(pngPtr);
   if(infoPtr == nullptr)
    { png_destroy_read_struct(&pngPtr, (png_infopp)nullptr, (png_infopp)nullptr);
      return false;
    }

   png_infop endInfo = png_create_info_struct(pngPtr);
   if(endInfo == nullptr)
    { png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)nullptr);
      return false;
    }

   PngImageRawData raw;

   raw.data = (unsigned char*) data -> getPtr ();
   raw.size = data -> getLength ();

   png_set_read_fn ( pngPtr, &raw, pngImageRead );
   png_read_info   ( pngPtr, infoPtr );

   png_uint_32 w, h;
   int bpp, colorType;
   png_get_IHDR(pngPtr, infoPtr, &w, &h, &bpp, &colorType, 0, 0, 0);
   //LOG_WARNING("Image: Load PNG w %i, h %i, bpp %i", w, h, bpp);
   // png формат может содержать 16 бит на канал, но нам нужно только 8, поэтому сужаем канал
   if (bpp == 16) png_set_strip_16(pngPtr);
   // преобразуем файл если он содержит палитру в нормальный RGB
   if (colorType == PNG_COLOR_TYPE_PALETTE && bpp <= 8) png_set_palette_to_rgb(pngPtr);
   // если в грэйскейле меньше бит на канал чем 8, то конвертим к нормальному 8-битному
   if (colorType == PNG_COLOR_TYPE_GRAY && bpp < 8) png_set_gray_1_2_4_to_8(pngPtr);
   // и добавляем полный альфа-канал
   if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(pngPtr);

   double gamma = 0.0f;
   // если есть информация о гамме в файле, то устанавливаем на 2.2
   if ( png_get_gAMA(pngPtr, infoPtr, &gamma) ) png_set_gamma(pngPtr, 2.2, gamma);
   // иначе ставим дефолтную гамму для файла в 0.45455 (good guess for GIF images on PCs)
   else png_set_gamma(pngPtr, 2.2, 0.45455);

   // после всех трансформаций, апдейтим информацию в библиотеке
   png_read_update_info(pngPtr, infoPtr);
   // опять получаем все размеры и параметры обновленной картинки
   png_get_IHDR(pngPtr, infoPtr, &w, &h, &bpp, &colorType, 0, 0, 0);

   // получаем кол-во каналов на пиксель 
   // может быть: 1 (GRAY, PALETTE), 2 (GRAY_ALPHA), 3 (RGB), 4 (RGB_ALPHA или RGB + filler byte)
   //png_byte channels = png_get_channels(pngPtr, infoPtr);
   // определяем кол-во байт нужных для того чтобы вместить строку
   png_uint_32 row_bytes = png_get_rowbytes(pngPtr, infoPtr);

   // теперь, мы можем выделить память чтобы вместить картинку
   png_byte* dat = new png_byte[row_bytes * h];
   // выделяем память, для указателей на каждую строку
   png_byte **row_pointers = new png_byte * [h];
   // сопоставляем массив указателей на строчки, с выделенными в памяти (res)
   // т.к. изображение перевернутое, то указатели идут снизу вверх
   for (unsigned int i = 0; i < h; i++)
    row_pointers[h - i - 1] = dat + i * row_bytes;

   // все, читаем картинку
   png_read_image(pngPtr, row_pointers);

   // читаем дополнительную информацию о файле (на самом деле игнорируем ее)
   png_read_end            ( pngPtr, endInfo );
   png_destroy_read_struct ( &pngPtr, &infoPtr, &endInfo );

   // освобождаем память от указателей на строки
   delete [] row_pointers;

   //  int rowBytes      = png_get_rowbytes     ( pngPtr, infoPtr );
   format = FORMAT_NONE;
   switch ( colorType )
    { case PNG_COLOR_TYPE_RGB:
       format = FORMAT_RGB8;
       break;
      case PNG_COLOR_TYPE_RGB_ALPHA:
       format = FORMAT_RGBA8;
       break;
      case PNG_COLOR_TYPE_GRAY:
      case PNG_COLOR_TYPE_GRAY_ALPHA:
       format = FORMAT_R8;
       break;
      case PNG_COLOR_TYPE_PALETTE:
       format = FORMAT_RGB8;
       break;
      default:
       break;
    }
   if(format == FORMAT_NONE)
    { png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)nullptr);
      return false;
    }

   LOG_SPAM("Image: Width %i, Height %i", w, h);

   width = (int)w;
   height = (int)h;
   pixels = (unsigned char*)dat;

   return true;
 }
