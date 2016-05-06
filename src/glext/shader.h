#pragma once

#include <GL/gl.h>
#include <GL/glext.h>
#include <string>
#include <glm/glm.hpp>
#include "object.h"
#include "glfun.h"

class Data;

class Shader : public Object
 { private:
    GLuint program;
    int num_init;
   public:
    Data *vsh;
    Data *fsh;

    Shader(const char *theName);
    ~Shader();

    int init();
    void clear();

    void bind()   { glUseProgram(program); }
    void unbind() { glUseProgram(0); }

    int getLocation(const char *Name)
     { return glGetUniformLocation(program, name);}

    template <typename Tp>
    bool setUniform(const char * name, Tp value);
    template <typename Tp>
    bool setUniform(int loc, Tp value);

    template <typename Tp>
    bool setUniformVec(const char * name, const Tp &value);
    template <typename Tp>
    bool setUniformVec(int loc, const Tp &value);

    template <typename Tp>
    bool setUniformMat(const char * name, const Tp &value);
    template <typename Tp>
    bool setUniformMat(int loc, const Tp &value);

    bool setTexture(const char* name, int textUnit);
    bool setTexture(int loc, int textUnit);

    static int maxVertexUniformComponents();
    static int maxVertexAttribs();
    static int maxFragmentTextureUnits();
    static int maxVertexTextureUnits();
    static int maxCombinedTextureUnits();
    static int maxVaryingFloats();
    static int maxFragmentUniformComponents();
    static int maxTextureCoords();

    static Meta::Base Instance;
 };

typedef Shader* PShader;
