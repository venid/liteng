
#include "texture.h"
#include <GL/gl.h>
#include "GL/glext.h"
#include "log.h"
#include "image.h"

META_METHODS(Texture)
META_PROPERTY(Texture)
META_OBJECT(Texture, Texture, &Object::Instance)

Texture :: Texture(const char* theName) : Object(theName)
 { m_target = 0;
   m_gl = 0;
   m_init = 0;
   image = nullptr;
   metaClass = &Instance;
 }

Texture :: ~Texture()
 { if(image) delete image; }

bool Texture :: init()
 { if(m_init == 0)
    { glGenTextures(1, &m_gl);
      glBindTexture(m_target, m_gl);
      glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      if (image->is2D())
       { glTexImage2D(m_target, 0, GL_RGBA, image->getWidth(), image->getHeight(),
                      0, GL_RGBA, GL_UNSIGNED_BYTE, image->getPixels());
         LOG_INFO("Texture: Registration texture \"%s\" complit.", getName());
       }

      glBindTexture(m_target, 0);
    }
   m_init++;
   return true;
 }

void Texture :: clear()
 { m_init--;
   if(m_init == 0)
    { glDeleteTextures(1, &m_gl);
      LOG_DEBUG("TEXTURE: clear gl.");
    }
 }

void Texture :: addImage(Image *img)
 { image = img;
   m_target = img->isCube()? GL_TEXTURE_CUBE_MAP :
              img->is3D()? GL_TEXTURE_3D :
              img->is2D()? GL_TEXTURE_2D : GL_TEXTURE_1D;
 }
