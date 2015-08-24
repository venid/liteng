#include "glfun.h"
#include <GL/glx.h>

struct token_string
 { GLuint Token;
   const char *String;
 };

static const struct token_string Errors[] =
 { { GL_NO_ERROR, "no error" },
   { GL_INVALID_ENUM, "invalid enumerant" },
   { GL_INVALID_VALUE, "invalid value" },
   { GL_INVALID_OPERATION, "invalid operation" },
   { GL_STACK_OVERFLOW, "stack overflow" },
   { GL_STACK_UNDERFLOW, "stack underflow" },
   { GL_OUT_OF_MEMORY, "out of memory" },
   { GL_INVALID_FRAMEBUFFER_OPERATION, "invalid framebuffer operation" },
   { (GLuint)~0, NULL } /* end of list indicator */
 };

const char* ErrorString(GLenum errorCode)
 { int i;
   for (i = 0; Errors[i].String; i++)
    if(Errors[i].Token == errorCode) return Errors[i].String;
   return 0;
 }

#define OPENGL_GET_PROC(p,n) \
  n = (p)glXGetProcAddressARB((const GLubyte *)#n); \
  if (nullptr == n) \
  { LOG_ERROR("Loading extension '%s' fail.", #n); \
          return false; \
  }

bool InitGL()
 { //vbo
   OPENGL_GET_PROC(PFNGLBINDBUFFERPROC, glBindBuffer);
   OPENGL_GET_PROC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
   OPENGL_GET_PROC(PFNGLGENBUFFERSPROC, glGenBuffers);
   OPENGL_GET_PROC(PFNGLBUFFERDATAPROC, glBufferData);
   OPENGL_GET_PROC(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
   OPENGL_GET_PROC(PFNGLBINDVERTEXBUFFERPROC, glBindVertexBuffer);
   OPENGL_GET_PROC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);

   //vao
   OPENGL_GET_PROC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
   OPENGL_GET_PROC(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
   OPENGL_GET_PROC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
 //  OPENGL_GET_PROC(PFNGLVERTEXATTRIBFORMATPROC, glVertexAttribFormat);
 //  OPENGL_GET_PROC(PFNGLVERTEXATTRIBBINDINGPROC, glVertexAttribBinding);
   OPENGL_GET_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);

   // shader
   OPENGL_GET_PROC(PFNGLATTACHSHADERPROC, glAttachShader);
   OPENGL_GET_PROC(PFNGLCOMPILESHADERPROC, glCompileShader);
   OPENGL_GET_PROC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
   OPENGL_GET_PROC(PFNGLCREATESHADERPROC, glCreateShader);
   OPENGL_GET_PROC(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
   OPENGL_GET_PROC(PFNGLDELETESHADERPROC, glDeleteShader);
   OPENGL_GET_PROC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
   OPENGL_GET_PROC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
   OPENGL_GET_PROC(PFNGLGETSHADERIVPROC, glGetShaderiv);
   OPENGL_GET_PROC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
   OPENGL_GET_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
   OPENGL_GET_PROC(PFNGLLINKPROGRAMPROC, glLinkProgram);
   OPENGL_GET_PROC(PFNGLSHADERSOURCEPROC, glShaderSource);
   OPENGL_GET_PROC(PFNGLUSEPROGRAMPROC, glUseProgram);
   OPENGL_GET_PROC(PFNGLUNIFORM1FPROC, glUniform1f);
   OPENGL_GET_PROC(PFNGLUNIFORM1IPROC, glUniform1i);
   OPENGL_GET_PROC(PFNGLUNIFORM1UIPROC, glUniform1ui);
   OPENGL_GET_PROC(PFNGLUNIFORM1FVPROC, glUniform1fv);
   OPENGL_GET_PROC(PFNGLUNIFORM2FVPROC, glUniform2fv);
   OPENGL_GET_PROC(PFNGLUNIFORM2IVPROC, glUniform2iv);
   OPENGL_GET_PROC(PFNGLUNIFORM2UIVPROC, glUniform2uiv);
   OPENGL_GET_PROC(PFNGLUNIFORM3FVPROC, glUniform3fv);
   OPENGL_GET_PROC(PFNGLUNIFORM3IVPROC, glUniform3iv);
   OPENGL_GET_PROC(PFNGLUNIFORM3UIVPROC, glUniform3uiv);
   OPENGL_GET_PROC(PFNGLUNIFORM4FVPROC, glUniform4fv);
   OPENGL_GET_PROC(PFNGLUNIFORM4IVPROC, glUniform4iv);
   OPENGL_GET_PROC(PFNGLUNIFORM4UIVPROC, glUniform4uiv);
   OPENGL_GET_PROC(PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv);
   OPENGL_GET_PROC(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv);
   OPENGL_GET_PROC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
   OPENGL_GET_PROC(PFNGLVALIDATEPROGRAMPROC, glValidateProgram);
   OPENGL_GET_PROC(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation);
   //rbo
   OPENGL_GET_PROC(PFNGLISRENDERBUFFERPROC, glIsRenderbuffer);
   OPENGL_GET_PROC(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
   OPENGL_GET_PROC(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers);
   OPENGL_GET_PROC(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
   OPENGL_GET_PROC(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);
   OPENGL_GET_PROC(PFNGLGETRENDERBUFFERPARAMETERIVPROC, glGetRenderbufferParameteriv);
   //fbo
   OPENGL_GET_PROC(PFNGLISFRAMEBUFFERPROC, glIsFramebuffer);
   OPENGL_GET_PROC(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
   OPENGL_GET_PROC(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
   OPENGL_GET_PROC(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
   OPENGL_GET_PROC(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
   OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTURE1DPROC, glFramebufferTexture1D);
   OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
   OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTURE3DPROC, glFramebufferTexture3D);
   OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTUREPROC, glFramebufferTexture);
   OPENGL_GET_PROC(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
   OPENGL_GET_PROC(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC, glGetFramebufferAttachmentParameteriv);

   return true;
 }

//GL_VERSION_1_4

// PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = nullptr;
// PFNGLMULTIDRAWARRAYSPROC   glMultiDrawArrays = nullptr;
// PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements = nullptr;
// PFNGLPOINTPARAMETERFPROC   glPointParameterf = nullptr;
// PFNGLPOINTPARAMETERFVPROC  glPointParameterfv = nullptr;
// PFNGLPOINTPARAMETERIPROC   glPointParameteri = nullptr;
// PFNGLPOINTPARAMETERIVPROC  glPointParameteriv = nullptr;
// PFNGLBLENDCOLORPROC        glBlendColor = nullptr;
// PFNGLBLENDEQUATIONPROC     glBlendEquation = nullptr;


//GL_VERSION_1_5

// PFNGLGENQUERIESPROC           glGenQueries = nullptr;
// PFNGLDELETEQUERIESPROC        glDeleteQueries = nullptr;
// PFNGLISQUERYPROC              glIsQuery = nullptr;
// PFNGLBEGINQUERYPROC           glBeginQuery = nullptr;
// PFNGLENDQUERYPROC             glEndQuery = nullptr;
// PFNGLGETQUERYIVPROC           glGetQueryiv = nullptr;
// PFNGLGETQUERYOBJECTIVPROC     glGetQueryObjectiv = nullptr;
// PFNGLGETQUERYOBJECTUIVPROC    glGetQueryObjectuiv = nullptr;
PFNGLBINDBUFFERPROC           glBindBuffer   = nullptr;
PFNGLDELETEBUFFERSPROC        glDeleteBuffers = nullptr;
PFNGLGENBUFFERSPROC           glGenBuffers   = nullptr;
// PFNGLISBUFFERPROC             glIsBuffer = nullptr;
PFNGLBUFFERDATAPROC           glBufferData   = nullptr;
PFNGLBUFFERSUBDATAPROC        glBufferSubData = nullptr;
// PFNGLGETBUFFERSUBDATAPROC     glGetBufferSubData = nullptr;
// PFNGLMAPBUFFERPROC            glMapBuffer = nullptr;
// PFNGLUNMAPBUFFERPROC          glUnmapBuffer = nullptr;
// PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = nullptr;
// PFNGLGETBUFFERPOINTERVPROC    glGetBufferPointerv = nullptr;

//GL_VERSION_2_0

// PFNGLBLENDEQUATIONSEPARATEPROC    glBlendEquationSeparate = nullptr;
// PFNGLDRAWBUFFERSPROC              glDrawBuffers = nullptr;
// PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate = nullptr;
// PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate = nullptr;
// PFNGLSTENCILMASKSEPARATEPROC      glStencilMaskSeparate = nullptr;
PFNGLATTACHSHADERPROC             glAttachShader  = nullptr;
PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation = nullptr;
PFNGLCOMPILESHADERPROC            glCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC            glCreateProgram = nullptr;
PFNGLCREATESHADERPROC             glCreateShader  = nullptr;
PFNGLDELETEPROGRAMPROC            glDeleteProgram = nullptr;
PFNGLDELETESHADERPROC             glDeleteShader  = nullptr;
// PFNGLDETACHSHADERPROC             glDetachShader = nullptr;
// PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray = nullptr;
// PFNGLGETACTIVEATTRIBPROC          glGetActiveAttrib = nullptr;
// PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform = nullptr;
// PFNGLGETATTACHEDSHADERSPROC       glGetAttachedShaders = nullptr;
// PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation = nullptr;
PFNGLGETPROGRAMIVPROC             glGetProgramiv      = nullptr;
PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog = nullptr;
PFNGLGETSHADERIVPROC              glGetShaderiv       = nullptr;
PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog  = nullptr;
// PFNGLGETSHADERSOURCEPROC          glGetShaderSource = nullptr;
PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation = nullptr;
// PFNGLGETUNIFORMFVPROC             glGetUniformfv = nullptr;
// PFNGLGETUNIFORMIVPROC             glGetUniformiv = nullptr;
// PFNGLGETVERTEXATTRIBDVPROC        glGetVertexAttribdv = nullptr;
// PFNGLGETVERTEXATTRIBFVPROC        glGetVertexAttribfv = nullptr;
// PFNGLGETVERTEXATTRIBIVPROC        glGetVertexAttribiv = nullptr;
// PFNGLGETVERTEXATTRIBPOINTERVPROC  glGetVertexAttribPointerv = nullptr;
// PFNGLISPROGRAMPROC                glIsProgram = nullptr;
// PFNGLISSHADERPROC                 glIsShader = nullptr;
PFNGLLINKPROGRAMPROC              glLinkProgram   = nullptr;
PFNGLSHADERSOURCEPROC             glShaderSource  = nullptr;
PFNGLUSEPROGRAMPROC               glUseProgram    = nullptr;
PFNGLUNIFORM1FPROC                glUniform1f     = nullptr;
// PFNGLUNIFORM2FPROC                glUniform2f = nullptr;
// PFNGLUNIFORM3FPROC                glUniform3f = nullptr;
// PFNGLUNIFORM4FPROC                glUniform4f = nullptr;
PFNGLUNIFORM1IPROC                glUniform1i     = nullptr;
// PFNGLUNIFORM2IPROC                glUniform2i = nullptr;
// PFNGLUNIFORM3IPROC                glUniform3i = nullptr;
// PFNGLUNIFORM4IPROC                glUniform4i = nullptr;
PFNGLUNIFORM1FVPROC               glUniform1fv    = nullptr;
PFNGLUNIFORM2FVPROC               glUniform2fv    = nullptr;
PFNGLUNIFORM3FVPROC               glUniform3fv    = nullptr;
PFNGLUNIFORM4FVPROC               glUniform4fv    = nullptr;
// PFNGLUNIFORM1IVPROC               glUniform1iv = nullptr;
PFNGLUNIFORM2IVPROC               glUniform2iv       = nullptr;
PFNGLUNIFORM3IVPROC               glUniform3iv       = nullptr;
PFNGLUNIFORM4IVPROC               glUniform4iv       = nullptr;
PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv = nullptr;
PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv = nullptr;
PFNGLVALIDATEPROGRAMPROC          glValidateProgram  = nullptr;
// PFNGLVERTEXATTRIB1DPROC           glVertexAttrib1d = nullptr;
// PFNGLVERTEXATTRIB1DVPROC          glVertexAttrib1dv = nullptr;
// PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f = nullptr;
// PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv = nullptr;
// PFNGLVERTEXATTRIB1SPROC           glVertexAttrib1s = nullptr;
// PFNGLVERTEXATTRIB1SVPROC          glVertexAttrib1sv = nullptr;
// PFNGLVERTEXATTRIB2DPROC           glVertexAttrib2d = nullptr;
// PFNGLVERTEXATTRIB2DVPROC          glVertexAttrib2dv = nullptr;
// PFNGLVERTEXATTRIB2FPROC           glVertexAttrib2f = nullptr;
// PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv = nullptr;
// PFNGLVERTEXATTRIB2SPROC           glVertexAttrib2s = nullptr;
// PFNGLVERTEXATTRIB2SVPROC          glVertexAttrib2sv = nullptr;
// PFNGLVERTEXATTRIB3DPROC           glVertexAttrib3d = nullptr;
// PFNGLVERTEXATTRIB3DVPROC          glVertexAttrib3dv = nullptr;
// PFNGLVERTEXATTRIB3FPROC           glVertexAttrib3f = nullptr;
// PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv = nullptr;
// PFNGLVERTEXATTRIB3SPROC           glVertexAttrib3s = nullptr;
// PFNGLVERTEXATTRIB3SVPROC          glVertexAttrib3sv = nullptr;
// PFNGLVERTEXATTRIB4NBVPROC         glVertexAttrib4Nbv = nullptr;
// PFNGLVERTEXATTRIB4NIVPROC         glVertexAttrib4Niv = nullptr;
// PFNGLVERTEXATTRIB4NSVPROC         glVertexAttrib4Nsv = nullptr;
// PFNGLVERTEXATTRIB4NUBPROC         glVertexAttrib4Nub = nullptr;
// PFNGLVERTEXATTRIB4NUBVPROC        glVertexAttrib4Nubv = nullptr;
// PFNGLVERTEXATTRIB4NUIVPROC        glVertexAttrib4Nuiv = nullptr;
// PFNGLVERTEXATTRIB4NUSVPROC        glVertexAttrib4Nusv = nullptr;
// PFNGLVERTEXATTRIB4BVPROC          glVertexAttrib4bv = nullptr;
// PFNGLVERTEXATTRIB4DPROC           glVertexAttrib4d = nullptr;
// PFNGLVERTEXATTRIB4DVPROC          glVertexAttrib4dv = nullptr;
// PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f = nullptr;
// PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv = nullptr;
// PFNGLVERTEXATTRIB4IVPROC          glVertexAttrib4iv = nullptr;
// PFNGLVERTEXATTRIB4SPROC           glVertexAttrib4s = nullptr;
// PFNGLVERTEXATTRIB4SVPROC          glVertexAttrib4sv = nullptr;
// PFNGLVERTEXATTRIB4UBVPROC         glVertexAttrib4ubv = nullptr;
// PFNGLVERTEXATTRIB4UIVPROC         glVertexAttrib4uiv = nullptr;
// PFNGLVERTEXATTRIB4USVPROC         glVertexAttrib4usv = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer  = nullptr;

//GL_VERSION_2_1

// PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv = nullptr;
// PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv = nullptr;
// PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv = nullptr;
// PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv = nullptr;
// PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv = nullptr;
// PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv = nullptr;

//GL_VERSION_3_0

// PFNGLCOLORMASKIPROC                  glColorMaski = nullptr;
// PFNGLGETBOOLEANI_VPROC               glGetBooleani_v = nullptr;
// PFNGLGETINTEGERI_VPROC               glGetIntegeri_v = nullptr;
// PFNGLENABLEIPROC                     glEnablei = nullptr;
// PFNGLDISABLEIPROC                    glDisablei = nullptr;
// PFNGLISENABLEDIPROC                  glIsEnabledi = nullptr;
// PFNGLBEGINTRANSFORMFEEDBACKPROC      glBeginTransformFeedback = nullptr;
// PFNGLENDTRANSFORMFEEDBACKPROC        glEndTransformFeedback = nullptr;
// PFNGLBINDBUFFERRANGEPROC             glBindBufferRange = nullptr;
// PFNGLBINDBUFFERBASEPROC              glBindBufferBase = nullptr;
// PFNGLTRANSFORMFEEDBACKVARYINGSPROC   glTransformFeedbackVaryings = nullptr;
// PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying = nullptr;
// PFNGLCLAMPCOLORPROC                  glClampColor = nullptr;
// PFNGLBEGINCONDITIONALRENDERPROC      glBeginConditionalRender = nullptr;
// PFNGLENDCONDITIONALRENDERPROC        glEndConditionalRender = nullptr;
// PFNGLVERTEXATTRIBIPOINTERPROC        glVertexAttribIPointer = nullptr;
// PFNGLGETVERTEXATTRIBIIVPROC          glGetVertexAttribIiv = nullptr;
// PFNGLGETVERTEXATTRIBIUIVPROC         glGetVertexAttribIuiv = nullptr;
// PFNGLVERTEXATTRIBI1IPROC             glVertexAttribI1i = nullptr;
// PFNGLVERTEXATTRIBI2IPROC             glVertexAttribI2i = nullptr;
// PFNGLVERTEXATTRIBI3IPROC             glVertexAttribI3i = nullptr;
// PFNGLVERTEXATTRIBI4IPROC             glVertexAttribI4i = nullptr;
// PFNGLVERTEXATTRIBI1UIPROC            glVertexAttribI1ui = nullptr;
// PFNGLVERTEXATTRIBI2UIPROC            glVertexAttribI2ui = nullptr;
// PFNGLVERTEXATTRIBI3UIPROC            glVertexAttribI3ui = nullptr;
// PFNGLVERTEXATTRIBI4UIPROC            glVertexAttribI4ui = nullptr;
// PFNGLVERTEXATTRIBI1IVPROC            glVertexAttribI1iv = nullptr;
// PFNGLVERTEXATTRIBI2IVPROC            glVertexAttribI2iv = nullptr;
// PFNGLVERTEXATTRIBI3IVPROC            glVertexAttribI3iv = nullptr;
// PFNGLVERTEXATTRIBI4IVPROC            glVertexAttribI4iv = nullptr;
// PFNGLVERTEXATTRIBI1UIVPROC           glVertexAttribI1uiv = nullptr;
// PFNGLVERTEXATTRIBI2UIVPROC           glVertexAttribI2uiv = nullptr;
// PFNGLVERTEXATTRIBI3UIVPROC           glVertexAttribI3uiv = nullptr;
// PFNGLVERTEXATTRIBI4UIVPROC           glVertexAttribI4uiv = nullptr;
// PFNGLVERTEXATTRIBI4BVPROC            glVertexAttribI4bv = nullptr;
// PFNGLVERTEXATTRIBI4SVPROC            glVertexAttribI4sv = nullptr;
// PFNGLVERTEXATTRIBI4UBVPROC           glVertexAttribI4ubv = nullptr;
// PFNGLVERTEXATTRIBI4USVPROC           glVertexAttribI4usv = nullptr;
// PFNGLGETUNIFORMUIVPROC               glGetUniformuiv = nullptr;
// PFNGLBINDFRAGDATALOCATIONPROC        glBindFragDataLocation = nullptr;
// PFNGLGETFRAGDATALOCATIONPROC         glGetFragDataLocation = nullptr;
PFNGLUNIFORM1UIPROC                  glUniform1ui = nullptr;
// PFNGLUNIFORM2UIPROC                  glUniform2ui = nullptr;
// PFNGLUNIFORM3UIPROC                  glUniform3ui = nullptr;
// PFNGLUNIFORM4UIPROC                  glUniform4ui = nullptr;
// PFNGLUNIFORM1UIVPROC                 glUniform1uiv = nullptr;
PFNGLUNIFORM2UIVPROC                 glUniform2uiv = nullptr;
PFNGLUNIFORM3UIVPROC                 glUniform3uiv = nullptr;
PFNGLUNIFORM4UIVPROC                 glUniform4uiv = nullptr;
// PFNGLTEXPARAMETERIIVPROC             glTexParameterIiv = nullptr;
// PFNGLTEXPARAMETERIUIVPROC            glTexParameterIuiv = nullptr;
// PFNGLGETTEXPARAMETERIIVPROC          glGetTexParameterIiv = nullptr;
// PFNGLGETTEXPARAMETERIUIVPROC         glGetTexParameterIuiv = nullptr;
// PFNGLCLEARBUFFERIVPROC               glClearBufferiv = nullptr;
// PFNGLCLEARBUFFERUIVPROC              glClearBufferuiv = nullptr;
// PFNGLCLEARBUFFERFVPROC               glClearBufferfv = nullptr;
// PFNGLCLEARBUFFERFIPROC               glClearBufferfi = nullptr;
// PFNGLGETSTRINGIPROC                  glGetStringi = nullptr;
PFNGLISRENDERBUFFERPROC              glIsRenderbuffer = nullptr;
PFNGLBINDRENDERBUFFERPROC            glBindRenderbuffer = nullptr;
PFNGLDELETERENDERBUFFERSPROC         glDeleteRenderbuffers = nullptr;
PFNGLGENRENDERBUFFERSPROC            glGenRenderbuffers = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC         glRenderbufferStorage = nullptr;
PFNGLGETRENDERBUFFERPARAMETERIVPROC  glGetRenderbufferParameteriv = nullptr;
PFNGLISFRAMEBUFFERPROC               glIsFramebuffer = nullptr;
PFNGLBINDFRAMEBUFFERPROC             glBindFramebuffer        = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC          glDeleteFramebuffers = nullptr;
PFNGLGENFRAMEBUFFERSPROC             glGenFramebuffers        = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC      glCheckFramebufferStatus = nullptr;
PFNGLFRAMEBUFFERTEXTURE1DPROC        glFramebufferTexture1D = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC        glFramebufferTexture2D   = nullptr;
PFNGLFRAMEBUFFERTEXTURE3DPROC        glFramebufferTexture3D = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC     glFramebufferRenderbuffer = nullptr;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = nullptr;
// PFNGLGENERATEMIPMAPPROC              glGenerateMipmap = nullptr;
// PFNGLBLITFRAMEBUFFERPROC             glBlitFramebuffer = nullptr;
// PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample = nullptr;
// PFNGLFRAMEBUFFERTEXTURELAYERPROC     glFramebufferTextureLayer = nullptr;
// PFNGLMAPBUFFERRANGEPROC              glMapBufferRange = nullptr;
// PFNGLFLUSHMAPPEDBUFFERRANGEPROC      glFlushMappedBufferRange = nullptr;
PFNGLBINDVERTEXARRAYPROC             glBindVertexArray    = nullptr;
PFNGLDELETEVERTEXARRAYSPROC          glDeleteVertexArrays = nullptr;
PFNGLGENVERTEXARRAYSPROC             glGenVertexArrays    = nullptr;
// PFNGLISVERTEXARRAYPROC               glIsVertexArray = nullptr;

//GL_VERSION_3_1



//GL_VERSION_3_2

//PFNGLDRAWELEMENTSBASEVERTEXPROC          glDrawElementsBaseVertex  = nullptr;
//PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC     glDrawRangeElementsBaseVertex  = nullptr;
//PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex = nullptr;
//PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC     glMultiDrawElementsBaseVertex  = nullptr;
//PFNGLPROVOKINGVERTEXPROC                 glProvokingVertex = nullptr;
//PFNGLFENCESYNCPROC                       glFenceSync = nullptr;
//PFNGLISSYNCPROC                          glIsSync    = nullptr;
//PFNGLDELETESYNCPROC                      glDeleteSync = nullptr;
//PFNGLCLIENTWAITSYNCPROC                  glClientWaitSync = nullptr;
//PFNGLWAITSYNCPROC                        glWaitSync       = nullptr;
//PFNGLGETINTEGER64VPROC                   glGetInteger64v  = nullptr;
//PFNGLGETSYNCIVPROC                       glGetSynciv      = nullptr;
//PFNGLGETINTEGER64I_VPROC                 glGetInteger64i_v = nullptr;
//PFNGLGETBUFFERPARAMETERI64VPROC          glGetBufferParameteri64v = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC              glFramebufferTexture     = nullptr;
//PFNGLTEXIMAGE2DMULTISAMPLEPROC           glTexImage2DMultisample  = nullptr;
//PFNGLTEXIMAGE3DMULTISAMPLEPROC           glTexImage3DMultisample  = nullptr;
//PFNGLGETMULTISAMPLEFVPROC                glGetMultisamplefv       = nullptr;
//PFNGLSAMPLEMASKIPROC                     glSampleMaski            = nullptr;

//GL_VERSION_3_3



//GL_VERSION_4_0



//GL_VERSION_4_1



//GL_VERSION_4_2

//PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC             glDrawArraysInstancedBaseInstance = nullptr;
//PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC           glDrawElementsInstancedBaseInstance = nullptr;
//PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance = nullptr;
//PFNGLGETINTERNALFORMATIVPROC                         glGetInternalformativ = nullptr;
//PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC              glGetActiveAtomicCounterBufferiv = nullptr;
//PFNGLBINDIMAGETEXTUREPROC                            glBindImageTexture = nullptr;
//PFNGLMEMORYBARRIERPROC                               glMemoryBarrier = nullptr;
//PFNGLTEXSTORAGE1DPROC                                glTexStorage1D = nullptr;
//PFNGLTEXSTORAGE2DPROC                                glTexStorage2D = nullptr;
//PFNGLTEXSTORAGE3DPROC                                glTexStorage3D = nullptr;
//PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC              glDrawTransformFeedbackInstanced = nullptr;
//PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC        glDrawTransformFeedbackStreamInstanced = nullptr;



//GL_VERSION_4_3

// PFNGLCLEARBUFFERDATAPROC                 glClearBufferData = nullptr;
// PFNGLCLEARBUFFERSUBDATAPROC              glClearBufferSubData = nullptr;
// PFNGLDISPATCHCOMPUTEPROC                 glDispatchCompute = nullptr;
// PFNGLDISPATCHCOMPUTEINDIRECTPROC         glDispatchComputeIndirect = nullptr;
// PFNGLCOPYIMAGESUBDATAPROC                glCopyImageSubData = nullptr;
// PFNGLFRAMEBUFFERPARAMETERIPROC           glFramebufferParameteri = nullptr;
// PFNGLGETFRAMEBUFFERPARAMETERIVPROC       glGetFramebufferParameteriv = nullptr;
// PFNGLGETINTERNALFORMATI64VPROC           glGetInternalformati64v = nullptr;
// PFNGLINVALIDATETEXSUBIMAGEPROC           glInvalidateTexSubImage = nullptr;
// PFNGLINVALIDATETEXIMAGEPROC              glInvalidateTexImage = nullptr;
// PFNGLINVALIDATEBUFFERSUBDATAPROC         glInvalidateBufferSubData = nullptr;
// PFNGLINVALIDATEBUFFERDATAPROC            glInvalidateBufferData = nullptr;
// PFNGLINVALIDATEFRAMEBUFFERPROC           glInvalidateFramebuffer = nullptr;
// PFNGLINVALIDATESUBFRAMEBUFFERPROC        glInvalidateSubFramebuffer = nullptr;
// PFNGLMULTIDRAWARRAYSINDIRECTPROC         glMultiDrawArraysIndirect = nullptr;
// PFNGLMULTIDRAWELEMENTSINDIRECTPROC       glMultiDrawElementsIndirect = nullptr;
// PFNGLGETPROGRAMINTERFACEIVPROC           glGetProgramInterfaceiv = nullptr;
// PFNGLGETPROGRAMRESOURCEINDEXPROC         glGetProgramResourceIndex = nullptr;
// PFNGLGETPROGRAMRESOURCENAMEPROC          glGetProgramResourceName = nullptr;
// PFNGLGETPROGRAMRESOURCEIVPROC            glGetProgramResourceiv = nullptr;
// PFNGLGETPROGRAMRESOURCELOCATIONPROC      glGetProgramResourceLocation = nullptr;
// PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glGetProgramResourceLocationIndex = nullptr;
// PFNGLSHADERSTORAGEBLOCKBINDINGPROC       glShaderStorageBlockBinding = nullptr;
// PFNGLTEXBUFFERRANGEPROC                  glTexBufferRange = nullptr;
// PFNGLTEXSTORAGE2DMULTISAMPLEPROC         glTexStorage2DMultisample = nullptr;
// PFNGLTEXSTORAGE3DMULTISAMPLEPROC         glTexStorage3DMultisample = nullptr;
// PFNGLTEXTUREVIEWPROC                     glTextureView = nullptr;
PFNGLBINDVERTEXBUFFERPROC                glBindVertexBuffer     = nullptr;
PFNGLVERTEXATTRIBFORMATPROC              glVertexAttribFormat   = nullptr;
// PFNGLVERTEXATTRIBIFORMATPROC             glVertexAttribIFormat = nullptr;
// PFNGLVERTEXATTRIBLFORMATPROC             glVertexAttribLFormat = nullptr;
PFNGLVERTEXATTRIBBINDINGPROC             glVertexAttribBinding  = nullptr;
// PFNGLVERTEXBINDINGDIVISORPROC            glVertexBindingDivisor = nullptr;
// PFNGLDEBUGMESSAGECONTROLPROC             glDebugMessageControl = nullptr;
// PFNGLDEBUGMESSAGEINSERTPROC              glDebugMessageInsert = nullptr;
// PFNGLDEBUGMESSAGECALLBACKPROC            glDebugMessageCallback = nullptr;
// PFNGLGETDEBUGMESSAGELOGPROC              glGetDebugMessageLog = nullptr;
// PFNGLPUSHDEBUGGROUPPROC                  glPushDebugGroup = nullptr;
// PFNGLPOPDEBUGGROUPPROC                   glPopDebugGroup = nullptr;
// PFNGLOBJECTLABELPROC                     glObjectLabel = nullptr;
// PFNGLGETOBJECTLABELPROC                  glGetObjectLabel = nullptr;
// PFNGLOBJECTPTRLABELPROC                  glObjectPtrLabel = nullptr;
// PFNGLGETOBJECTPTRLABELPROC               glGetObjectPtrLabel = nullptr;
