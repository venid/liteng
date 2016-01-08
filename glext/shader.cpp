#include "shader.h"
#include "log.h"
#include "data.h"
#include <glm/gtc/type_ptr.hpp>

META_METHODS(Shader)
META_PROPERTY(Shader)
META_OBJECT(Shader, Shader, &Object::Instance)

/// -----------------------------------------------------------------------------

Shader ::Shader(const char *theName) : Object(theName)
 { program = 0;
   num_init = 0;
   vsh = nullptr;
   fsh = nullptr;
   metaClass = &Instance;
 }

Shader :: ~Shader()
 { if(vsh) delete vsh;
   if(fsh) delete fsh;
 }

int Shader :: init()
 { GLint status, len;
   GLuint shader;
   const char* body;
   GLchar buffer[1024];

   if(num_init > 0) return 1;

   if(vsh && fsh)
    { program = glCreateProgram();
      shader = glCreateShader(GL_VERTEX_SHADER);
      body = (const char*)vsh->getPtr(0);
      len = (GLint)vsh->getLength();
      glShaderSource(shader, 1, &body, &len);
      glCompileShader(shader);
      glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
      if(status != GL_TRUE)
       { glGetShaderInfoLog(shader, 1024, &len, buffer);
         LOG_WARNING("GLSL(vert): %s.", (const char*)buffer);
         return 0;
       }
      glAttachShader(program, shader);
      glBindAttribLocation(program, 0, "in_vert");
      glBindAttribLocation(program, 1, "in_norm");
      glBindAttribLocation(program, 2, "in_coord");
      glBindAttribLocation(program, 3, "in_color");
      glBindAttribLocation(program, 4, "in_tangent");
      glBindAttribLocation(program, 5, "in_binormal");
      glDeleteShader(shader);

      shader = glCreateShader(GL_FRAGMENT_SHADER);
      body = (const char*)fsh->getPtr(0);
      len = (GLint)fsh->getLength();
      glShaderSource(shader, 1, &body, &len);
      glCompileShader(shader);
      glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
      if(status != GL_TRUE)
       { glGetShaderInfoLog(shader, 1024, &len, buffer);
         LOG_WARNING("GLSL(frag): %s.", (const char*)buffer);
         return 0;
       }
      glAttachShader(program, shader);
      glDeleteShader(shader);
    }
   else return 0;

   glLinkProgram(program);
   glGetProgramiv(program, GL_LINK_STATUS, &status);
   if(status != GL_TRUE)
    { glGetProgramInfoLog(program, 1024, &len, buffer);
      LOG_WARNING("GLSL(link): %s.", (const char*)buffer);
      return 0;
    }
   glValidateProgram(program);
   glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
   if(status != GL_TRUE)
    { glGetProgramInfoLog(program, 1024, &len, buffer);
      LOG_WARNING("GLSL(valid): %s.", (const char*)buffer);
      return 0;
    }

   num_init ++;
   LOG_INFO("Shader: Linking shader programs \"%s\" complete.", getName());
   return 1;
 }

void Shader :: clear()
 { num_init --;
   if(program && (num_init < 1))
    { glDeleteProgram(program); program = 0; }
 }

// ------------------------------------------------------------------------------

template<>
bool Shader :: setUniform<float>(const char * name, float value)
 { int loc = glGetUniformLocation(program, name);
   if ( loc < 0 ) return false;
   glUniform1f(loc, value);
   return true;
 }

template<>
bool Shader :: setUniform<float>(int loc, float value)
 { if(loc < 0) return false;
   glUniform1f(loc, value);
   return true;
 }

template<>
bool Shader :: setUniform<int>(const char * name, int value)
 { int loc = glGetUniformLocation(program, name);
   if ( loc < 0 ) return false;
   glUniform1i( loc, value );
   return true;
 }

template<>
bool Shader :: setUniform<int>(int loc, int value)
 { if(loc < 0) return false;
   glUniform1i(loc, value);
   return true;
 }

template<>
bool Shader :: setUniform<unsigned int>(const char * name, unsigned int value)
 { int loc = glGetUniformLocation(program, name);
   if ( loc < 0 ) return false;
   glUniform1ui( loc, value );
   return true;
 }

template<>
bool Shader :: setUniform<unsigned int>(int loc, unsigned int value)
 { if(loc < 0) return false;
   glUniform1ui(loc, value);
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::vec2>(const char *name, const glm::vec2 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc < 0) return false;
   glUniform2fv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::vec2>(int loc, const glm::vec2 &value)
 { if(loc < 0) return false;
   glUniform2fv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::ivec2>(const char *name, const glm::ivec2 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc < 0) return false;
   glUniform2iv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::ivec2>(int loc, const glm::ivec2 &value)
 { if(loc < 0) return false;
   glUniform2iv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::uvec2>(const char *name, const glm::uvec2 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc < 0) return false;
   glUniform2uiv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::uvec2>(int loc, const glm::uvec2 &value)
 { if(loc < 0) return false;
   glUniform2uiv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::vec3>(const char *name, const glm::vec3 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc == -1) return false;
   glUniform3fv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::vec3>(int loc, const glm::vec3 &value)
 { if(loc < 0) return false;
   glUniform3fv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::ivec3>(const char *name, const glm::ivec3 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc < 0) return false;
   glUniform3iv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::ivec3>(int loc, const glm::ivec3 &value)
 { if(loc < 0) return false;
   glUniform3iv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::uvec3>(const char *name, const glm::uvec3 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc == -1) return false;
   glUniform3uiv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::uvec3>(int loc, const glm::uvec3 &value)
 { if(loc < 0) return false;
   glUniform3uiv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::vec4>(const char *name, const glm::vec4 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc < 0) return false;
   glUniform4fv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::vec4>(int loc, const glm::vec4 &value)
 { if(loc < 0) return false;
   glUniform4fv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::ivec4>(const char *name, const glm::ivec4 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc < 0) return false;
   glUniform4iv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::ivec4>(int loc, const glm::ivec4 &value)
 { if(loc < 0) return false;
   glUniform4iv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::uvec4>(const char *name, const glm::uvec4 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc < 0) return false;
   glUniform4uiv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformVec<glm::uvec4>(int loc, const glm::uvec4 &value)
 { if(loc < 0) return false;
   glUniform4uiv(loc, 1, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformMat<glm::mat2>(const char *name, const glm::mat2 &value)
 { int loc = glGetUniformLocation(program, name);
   if ( loc < 0 ) return false;
   glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformMat<glm::mat2>(int loc, const glm::mat2 &value)
 { if(loc < 0) return false;
   glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformMat<glm::mat3>(const char *name, const glm::mat3 &value)
 { int loc = glGetUniformLocation(program, name);
   if ( loc < 0 ) return false;
   glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformMat<glm::mat3>(int loc, const glm::mat3 &value)
 { if(loc < 0) return false;
   glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformMat<glm::mat4>(const char *name, const glm::mat4 &value)
 { int loc = glGetUniformLocation(program, name);
   if(loc < 0) return false;
   glUniformMatrix4fv( loc, 1, GL_FALSE, glm::value_ptr(value));
   return true;
 }

template<>
bool Shader :: setUniformMat<glm::mat4>(int loc, const glm::mat4 &value)
 { if(loc < 0) return false;
   glUniformMatrix4fv( loc, 1, GL_FALSE, glm::value_ptr(value));
   return true;
 }

bool Shader :: setTexture(const char* name, int textUnit)
 { int loc = glGetUniformLocation(program, name);
   if(loc < 0) return false;
   glUniform1i(loc, textUnit);
   return true;
 }

bool Shader :: setTexture(int loc, int textUnit)
 { if(loc < 0) return false;
   glUniform1i(loc, textUnit);
   return true;
 }

// -----------------------------------------------------------------------------------------------

int Shader :: maxVertexUniformComponents()
 { GLint maxVertexUniformComponents;
   glGetIntegerv ( GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents );
   return maxVertexUniformComponents;
 }

int Shader :: maxVertexAttribs()
 { GLint maxVertexAttribs;
   glGetIntegerv ( GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs );
   return maxVertexAttribs;
 }

int Shader :: maxFragmentTextureUnits()
 { GLint maxFragmentTextureUnits;
   glGetIntegerv ( GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragmentTextureUnits );
   return maxFragmentTextureUnits;
 }

int Shader :: maxVertexTextureUnits()
 { GLint maxVertexTextureUnits;
   glGetIntegerv ( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextureUnits );
   return maxVertexTextureUnits;
 }

int Shader :: maxCombinedTextureUnits()
 { GLint maxCombinedTextureUnits;
   glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureUnits );
   return maxCombinedTextureUnits;
 }

int Shader :: maxVaryingFloats()
 { GLint maxVaryingFloats;
   glGetIntegerv ( GL_MAX_VARYING_FLOATS, &maxVaryingFloats );
   return maxVaryingFloats;
 }

int Shader :: maxFragmentUniformComponents()
 { GLint maxFragmentUniformComponents;
   glGetIntegerv ( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniformComponents );
   return maxFragmentUniformComponents;
 }

int Shader :: maxTextureCoords()
 { GLint maxTextureCoords;
   glGetIntegerv ( GL_MAX_TEXTURE_COORDS, &maxTextureCoords );
   return maxTextureCoords;
 }
