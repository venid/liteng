//
// C++ wrapper for framebuffer object
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#pragma once

#include <GL/gl.h>
#include <GL/glext.h>

class FrameBuffer
 { private:
    int m_flags;
    int m_width;
    int m_height;

    GLuint m_frame;
    GLuint m_color[8];
    GLuint m_depth;
    GLuint m_stencil;
    GLint m_saveViewport[4];

   public:
    FrameBuffer(int flags = 0);
    ~FrameBuffer() {}

    void setSize(int width, int height);
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    bool hasStencil() const { return m_stencil != 0; }
    bool hasDepth() const { return m_depth != 0; }

    unsigned getColorBuffer(int no = 0) const { return m_color[no]; }
    unsigned getDepthBuffer() const { return m_depth; }

    bool isOk() const;
    bool init();
    void clear();
    bool bind();
    bool bindFace(int face);
    bool unbind(bool genMipmaps = false,  GLenum target = GL_TEXTURE_2D);

    bool attachColorTexture (GLenum target, unsigned texId, int no = 0);
    bool attachDepthTexture (GLenum target, unsigned texId);
    bool attach3DTextureSlice (unsigned texId, int zOffs, int no = 0);

    bool detachColorTexture (GLenum target)
     { return attachColorTexture (target, 0); }
    bool detachDepthTexture (GLenum target)
     { return attachDepthTexture (target, 0); }

    // create texture for attaching
    unsigned createColorTexture (GLenum format = GL_RGBA,
                                 GLenum internalFormat = GL_RGBA8,
                                 GLenum clamp = GL_REPEAT,
                                 int filter = filterLinear);

    unsigned createColorRectTexture (GLenum format = GL_RGBA,
                                     GLenum internalFormat = GL_RGBA8);

    unsigned createCubemapTexture (GLenum format = GL_RGBA,
                                   GLenum internalFormat = GL_RGBA8,
                                   GLenum clamp = GL_CLAMP_TO_EDGE,
                                   int filter = filterLinear);

    unsigned create3DTexture (int depth,
                              GLenum format = GL_RGBA,
                              GLenum internalFormat = GL_RGBA8,
                              GLenum clamp = GL_REPEAT,
                              int filter = filterLinear);

    // mipmapping support
    void buildMipmaps (GLenum target = GL_TEXTURE_2D) const;

    static int maxColorAttached();
    static int maxSize();

    enum               // flags for depth and stencil buffers
     { depth16 = 1,    // 16-bit depth buffer
       depth24 = 2,    // 24-bit depth buffer
       depth32 = 4,    // 32-bit depth buffer

       stencil1  = 16, // 1-bit stencil buffer
       stencil4  = 32, // 4-bit stencil buffer
       stencil8  = 64, // 8-bit stencil buffer
       stencil16 = 128 // 16-bit stencil buffer
     };

    enum         // filter modes
     { filterNearest = 0,
       filterLinear  = 1,
       filterMipmap  = 2
     };
 };
