
#include "texture.h"
#include <GL/gl.h>
#include "GL/glext.h"
#include "data.h"
#include "log.h"

META_METHODS(Texture)
META_PROPERTY(Texture)
META_OBJECT(Texture, Texture, &Object::Instance)

Texture :: Texture(const char* theName) : Object(theName)
 { m_target = 0;
   m_gl = 0;
   m_init = 0;
   metaClass = &Instance;
 }

Texture :: ~Texture()
 { }

bool Texture :: init()
 { if(m_init == 0)
    { glGenTextures(1, &m_gl);
      LOG_DEBUG("TEXTURE: create complit.");
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
