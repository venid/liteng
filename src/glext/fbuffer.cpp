
#include "fbuffer.h"
#include "glfun.h"

FrameBuffer :: FrameBuffer(int flags)
 { m_width = 0;
   m_height = 0;
   m_flags = flags;
   m_frame = 0;
   m_depth = 0;
   m_stencil = 0;

   for(int i = 0; i < 8; i++)
    m_color[i] = 0;
 }

void FrameBuffer :: setSize(int width, int height)
 { m_width = width;
   m_height = height;
 }

bool FrameBuffer :: isOk() const
 { GLint currentFb;

   glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFb);
   if(currentFb != (GLint)m_frame)
    { glBindFramebuffer(GL_FRAMEBUFFER, m_frame);
      glReadBuffer(GL_COLOR_ATTACHMENT0);
    }

   int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

   if(currentFb != (GLint)m_frame)
    glBindFramebuffer(GL_FRAMEBUFFER, currentFb);

   return status == GL_FRAMEBUFFER_COMPLETE;
 }

bool FrameBuffer :: init()
 { if(m_width <= 0 || m_height <= 0) return false;

   glGenFramebuffers (1, &m_frame);
   glBindFramebuffer (GL_FRAMEBUFFER_EXT, m_frame);

   int depthFormat   = 0;
   int stencilFormat = 0;

   if (m_flags & depth16) depthFormat = GL_DEPTH_COMPONENT16;
   else
    if (m_flags & depth24) depthFormat = GL_DEPTH_COMPONENT24;
    else
     if (m_flags & depth32) depthFormat = GL_DEPTH_COMPONENT32;

   if (m_flags & stencil1) stencilFormat = GL_STENCIL_INDEX1;
   else
    if (m_flags & stencil4) stencilFormat = GL_STENCIL_INDEX4;
    else
     if (m_flags & stencil8) stencilFormat = GL_STENCIL_INDEX8;
     else
      if (m_flags & stencil16) stencilFormat = GL_STENCIL_INDEX16;


   if (depthFormat != 0)
    { glGenRenderbuffers (1, &m_depth);
      glBindRenderbuffer (GL_RENDERBUFFER, m_depth);
      glRenderbufferStorage (GL_RENDERBUFFER, depthFormat, m_width, m_height);
      glFramebufferRenderbuffer (GL_FRAMEBUFFER,  GL_DEPTH_ATTACHMENT,
                                 GL_RENDERBUFFER, m_depth);
    }

   if (stencilFormat != 0)
    { glGenRenderbuffers (1, &m_stencil);
      glBindRenderbuffer (GL_RENDERBUFFER, m_stencil);
      glRenderbufferStorage (GL_RENDERBUFFER, stencilFormat, m_width, m_height);
      glFramebufferRenderbuffer (GL_FRAMEBUFFER,  GL_STENCIL_ATTACHMENT,
                                 GL_RENDERBUFFER, m_stencil);
    }

   GLenum status = glCheckFramebufferStatus (GL_FRAMEBUFFER);

   glBindFramebuffer (GL_FRAMEBUFFER, 0);

   return status == GL_FRAMEBUFFER_COMPLETE;
 }

void FrameBuffer :: clear()
 { if(m_depth != 0) glDeleteRenderbuffers(1, &m_depth);
   if(m_stencil != 0) glDeleteRenderbuffers(1, &m_stencil);
   if(m_frame != 0) glDeleteFramebuffers(1, &m_frame);
 }

bool FrameBuffer :: bind()
 { if (m_frame == 0) return false;

   glFlush ();
   // save current viewport
   glGetIntegerv (GL_VIEWPORT, m_saveViewport);
   glBindFramebuffer (GL_FRAMEBUFFER, m_frame);
   glReadBuffer (GL_COLOR_ATTACHMENT0);
   glDrawBuffer (GL_COLOR_ATTACHMENT0);
   glViewport (0, 0, m_width, m_height);

   return true;
 }

bool FrameBuffer :: bindFace (int face)
 { glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_color[0], 0);

   return bind ();
 }

bool FrameBuffer :: unbind (bool genMipmaps, GLenum target)
 { if (m_frame == 0) return false;

   glFlush ();

   if (genMipmaps) glGenerateMipmap (target);

   glBindFramebuffer (GL_FRAMEBUFFER, 0);
   glViewport (m_saveViewport[0], m_saveViewport[1], m_saveViewport[2], m_saveViewport[3]);

   return true;
 }

bool FrameBuffer :: attachColorTexture (GLenum target, unsigned texId, int no)
 { if (m_frame == 0) return false;

   if (target != GL_TEXTURE_2D && target != GL_TEXTURE_RECTANGLE &&
       (target < GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB ||
        target > GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB)) return false;

   m_color[no] = texId;
   glBindTexture (target, m_color[no]);
   glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + no, target, texId, 0);

   return true;
 }

bool FrameBuffer :: attach3DTextureSlice (unsigned texId, int zOffs, int no)
 { if (m_frame == 0) return false;

   m_color[no] = texId;
   glBindTexture (GL_TEXTURE_3D, m_color[no]);
   glFramebufferTexture3D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + no, GL_TEXTURE_3D, texId, 0, zOffs);

   return true;
 }

bool FrameBuffer :: attachDepthTexture (GLenum target, unsigned texId)
 { if (m_frame == 0) return false;

   m_depth = texId;
   glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, m_depth, 0);

   return true;
 }

unsigned FrameBuffer :: createColorTexture (GLenum format, GLenum internalFormat,
                                            GLenum clamp, int filter )
 { GLuint tex;

   glGenTextures (1, &tex);
   glBindTexture (GL_TEXTURE_2D, tex);
   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);     // set 1-byte alignment
   glTexImage2D (GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0,
                 format, GL_UNSIGNED_BYTE, nullptr);

   switch(filter)
    { case filterNearest:
       glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       break;
      case filterLinear:
       glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       break;
      case filterMipmap:
       glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
       break;
    }

   glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
   glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);

   return tex;
 }

unsigned FrameBuffer :: createCubemapTexture (GLenum format, GLenum internalFormat,
                                              GLenum clamp, int filter)
 { if (m_width != m_height) return 0;

   GLuint tex;

   glGenTextures (1, &tex);
   glBindTexture (GL_TEXTURE_CUBE_MAP, tex);
   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);                         // set 1-byte alignment
   glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, clamp);
   glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, clamp);
   glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, clamp);

   for (int side = 0; side < 6; side++ )
    glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, internalFormat,
                  m_width, m_width, 0, format, GL_UNSIGNED_BYTE, nullptr);

   switch(filter)
    { case filterNearest:
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       break;
      case filterLinear:
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       break;
      case filterMipmap:
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
       break;
    }

   return tex;
 }

unsigned FrameBuffer :: create3DTexture (int depth, GLenum format, GLenum internalFormat,
                                         GLenum clamp, int filter)
 { if (m_width != m_height) return 0;

   GLuint tex;

   glGenTextures (1, &tex);
   glBindTexture (GL_TEXTURE_3D, tex);
   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);                         // set 1-byte alignment
   glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, clamp);
   glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, clamp);
   glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, clamp);

   glTexImage3D (GL_TEXTURE_3D, 0, internalFormat, m_width, m_width, depth, 0,
                 format, GL_UNSIGNED_BYTE, nullptr);

   switch(filter)
    { case filterNearest:
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       break;
      case filterLinear:
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       break;
      case filterMipmap:
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
       break;
    }

   return tex;
 }

unsigned FrameBuffer :: createColorRectTexture (GLenum format, GLenum internalFormat)
 { GLuint tex;

   glGenTextures (1, &tex);
   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);                   // set 1-byte alignment
   glBindTexture (GL_TEXTURE_RECTANGLE, tex);
   glTexImage2D (GL_TEXTURE_RECTANGLE, 0, internalFormat, m_width, m_height, 0,
                 format, GL_UNSIGNED_BYTE, nullptr);

   glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

   return tex;
 }

void FrameBuffer :: buildMipmaps (GLenum target) const
 { // be sure we're unbound
   glBindTexture (target, getColorBuffer());
   glGenerateMipmap (target);
 }

int FrameBuffer :: maxColorAttached()
 { GLint n;
   glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &n);
   return n;
 }

int FrameBuffer :: maxSize()
 { GLint sz;
   glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &sz);
   return sz;
 }
