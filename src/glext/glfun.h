
#pragma once

#include <GL/gl.h>
#include <GL/glext.h>
#include "log.h"

const char* ErrorString(GLenum errorCode);

inline bool CheckErrorGL()
 { GLenum err;
   if ((err = glGetError()) == GL_NO_ERROR) return true;
   else
    { LOG_ERROR("OpenGL: %s", ErrorString(err));
      return false;
    }
 }

bool InitGL();

//GL_VERSION_1_4

extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
extern PFNGLMULTIDRAWARRAYSPROC   glMultiDrawArrays;
extern PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
extern PFNGLPOINTPARAMETERFPROC   glPointParameterf;
extern PFNGLPOINTPARAMETERFVPROC  glPointParameterfv;
extern PFNGLPOINTPARAMETERIPROC   glPointParameteri;
extern PFNGLPOINTPARAMETERIVPROC  glPointParameteriv;
//extern PFNGLBLENDCOLORPROC        glBlendColor;
//extern PFNGLBLENDEQUATIONPROC     glBlendEquation;

//GL_VERSION_1_5

extern PFNGLGENQUERIESPROC           glGenQueries;
extern PFNGLDELETEQUERIESPROC        glDeleteQueries;
extern PFNGLISQUERYPROC              glIsQuery;
extern PFNGLBEGINQUERYPROC           glBeginQuery;
extern PFNGLENDQUERYPROC             glEndQuery;
extern PFNGLGETQUERYIVPROC           glGetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC     glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC    glGetQueryObjectuiv;
extern PFNGLBINDBUFFERPROC           glBindBuffer;
extern PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
extern PFNGLGENBUFFERSPROC           glGenBuffers;
extern PFNGLISBUFFERPROC             glIsBuffer;
extern PFNGLBUFFERDATAPROC           glBufferData;
extern PFNGLBUFFERSUBDATAPROC        glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC     glGetBufferSubData;
extern PFNGLMAPBUFFERPROC            glMapBuffer;
extern PFNGLUNMAPBUFFERPROC          glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC    glGetBufferPointerv;

//GL_VERSION_2_0

extern PFNGLBLENDEQUATIONSEPARATEPROC    glBlendEquationSeparate;
extern PFNGLDRAWBUFFERSPROC              glDrawBuffers;
extern PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC      glStencilMaskSeparate;
extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLDETACHSHADERPROC             glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC          glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC       glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC          glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC             glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC             glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC        glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC        glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC        glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC  glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC                glIsProgram;
extern PFNGLISSHADERPROC                 glIsShader;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLUNIFORM1FPROC                glUniform1f;
extern PFNGLUNIFORM2FPROC                glUniform2f;
extern PFNGLUNIFORM3FPROC                glUniform3f;
extern PFNGLUNIFORM4FPROC                glUniform4f;
extern PFNGLUNIFORM1IPROC                glUniform1i;
extern PFNGLUNIFORM2IPROC                glUniform2i;
extern PFNGLUNIFORM3IPROC                glUniform3i;
extern PFNGLUNIFORM4IPROC                glUniform4i;
extern PFNGLUNIFORM1FVPROC               glUniform1fv;
extern PFNGLUNIFORM2FVPROC               glUniform2fv;
extern PFNGLUNIFORM3FVPROC               glUniform3fv;
extern PFNGLUNIFORM4FVPROC               glUniform4fv;
extern PFNGLUNIFORM1IVPROC               glUniform1iv;
extern PFNGLUNIFORM2IVPROC               glUniform2iv;
extern PFNGLUNIFORM3IVPROC               glUniform3iv;
extern PFNGLUNIFORM4IVPROC               glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
extern PFNGLVALIDATEPROGRAMPROC          glValidateProgram;
extern PFNGLVERTEXATTRIB1DPROC           glVertexAttrib1d;
extern PFNGLVERTEXATTRIB1DVPROC          glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1SPROC           glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1SVPROC          glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB2DPROC           glVertexAttrib2d;
extern PFNGLVERTEXATTRIB2DVPROC          glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB2FPROC           glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2SPROC           glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2SVPROC          glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB3DPROC           glVertexAttrib3d;
extern PFNGLVERTEXATTRIB3DVPROC          glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB3FPROC           glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3SPROC           glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3SVPROC          glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB4NBVPROC         glVertexAttrib4Nbv;
extern PFNGLVERTEXATTRIB4NIVPROC         glVertexAttrib4Niv;
extern PFNGLVERTEXATTRIB4NSVPROC         glVertexAttrib4Nsv;
extern PFNGLVERTEXATTRIB4NUBPROC         glVertexAttrib4Nub;
extern PFNGLVERTEXATTRIB4NUBVPROC        glVertexAttrib4Nubv;
extern PFNGLVERTEXATTRIB4NUIVPROC        glVertexAttrib4Nuiv;
extern PFNGLVERTEXATTRIB4NUSVPROC        glVertexAttrib4Nusv;
extern PFNGLVERTEXATTRIB4BVPROC          glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4DPROC           glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4DVPROC          glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4IVPROC          glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4SPROC           glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4SVPROC          glVertexAttrib4sv;
extern PFNGLVERTEXATTRIB4UBVPROC         glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4UIVPROC         glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4USVPROC         glVertexAttrib4usv;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;

//GL_VERSION_2_1

extern PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

//GL_VERSION_3_0

extern PFNGLCOLORMASKIPROC                  glColorMaski;
extern PFNGLGETBOOLEANI_VPROC               glGetBooleani_v;
extern PFNGLGETINTEGERI_VPROC               glGetIntegeri_v;
extern PFNGLENABLEIPROC                     glEnablei;
extern PFNGLDISABLEIPROC                    glDisablei;
extern PFNGLISENABLEDIPROC                  glIsEnabledi;
extern PFNGLBEGINTRANSFORMFEEDBACKPROC      glBeginTransformFeedback;
extern PFNGLENDTRANSFORMFEEDBACKPROC        glEndTransformFeedback;
extern PFNGLBINDBUFFERRANGEPROC             glBindBufferRange;
extern PFNGLBINDBUFFERBASEPROC              glBindBufferBase;
extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC   glTransformFeedbackVaryings;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
extern PFNGLCLAMPCOLORPROC                  glClampColor;
extern PFNGLBEGINCONDITIONALRENDERPROC      glBeginConditionalRender;
extern PFNGLENDCONDITIONALRENDERPROC        glEndConditionalRender;
extern PFNGLVERTEXATTRIBIPOINTERPROC        glVertexAttribIPointer;
extern PFNGLGETVERTEXATTRIBIIVPROC          glGetVertexAttribIiv;
extern PFNGLGETVERTEXATTRIBIUIVPROC         glGetVertexAttribIuiv;
extern PFNGLVERTEXATTRIBI1IPROC             glVertexAttribI1i;
extern PFNGLVERTEXATTRIBI2IPROC             glVertexAttribI2i;
extern PFNGLVERTEXATTRIBI3IPROC             glVertexAttribI3i;
extern PFNGLVERTEXATTRIBI4IPROC             glVertexAttribI4i;
extern PFNGLVERTEXATTRIBI1UIPROC            glVertexAttribI1ui;
extern PFNGLVERTEXATTRIBI2UIPROC            glVertexAttribI2ui;
extern PFNGLVERTEXATTRIBI3UIPROC            glVertexAttribI3ui;
extern PFNGLVERTEXATTRIBI4UIPROC            glVertexAttribI4ui;
extern PFNGLVERTEXATTRIBI1IVPROC            glVertexAttribI1iv;
extern PFNGLVERTEXATTRIBI2IVPROC            glVertexAttribI2iv;
extern PFNGLVERTEXATTRIBI3IVPROC            glVertexAttribI3iv;
extern PFNGLVERTEXATTRIBI4IVPROC            glVertexAttribI4iv;
extern PFNGLVERTEXATTRIBI1UIVPROC           glVertexAttribI1uiv;
extern PFNGLVERTEXATTRIBI2UIVPROC           glVertexAttribI2uiv;
extern PFNGLVERTEXATTRIBI3UIVPROC           glVertexAttribI3uiv;
extern PFNGLVERTEXATTRIBI4UIVPROC           glVertexAttribI4uiv;
extern PFNGLVERTEXATTRIBI4BVPROC            glVertexAttribI4bv;
extern PFNGLVERTEXATTRIBI4SVPROC            glVertexAttribI4sv;
extern PFNGLVERTEXATTRIBI4UBVPROC           glVertexAttribI4ubv;
extern PFNGLVERTEXATTRIBI4USVPROC           glVertexAttribI4usv;
extern PFNGLGETUNIFORMUIVPROC               glGetUniformuiv;
extern PFNGLBINDFRAGDATALOCATIONPROC        glBindFragDataLocation;
extern PFNGLGETFRAGDATALOCATIONPROC         glGetFragDataLocation;
extern PFNGLUNIFORM1UIPROC                  glUniform1ui;
extern PFNGLUNIFORM2UIPROC                  glUniform2ui;
extern PFNGLUNIFORM3UIPROC                  glUniform3ui;
extern PFNGLUNIFORM4UIPROC                  glUniform4ui;
extern PFNGLUNIFORM1UIVPROC                 glUniform1uiv;
extern PFNGLUNIFORM2UIVPROC                 glUniform2uiv;
extern PFNGLUNIFORM3UIVPROC                 glUniform3uiv;
extern PFNGLUNIFORM4UIVPROC                 glUniform4uiv;
extern PFNGLTEXPARAMETERIIVPROC             glTexParameterIiv;
extern PFNGLTEXPARAMETERIUIVPROC            glTexParameterIuiv;
extern PFNGLGETTEXPARAMETERIIVPROC          glGetTexParameterIiv;
extern PFNGLGETTEXPARAMETERIUIVPROC         glGetTexParameterIuiv;
extern PFNGLCLEARBUFFERIVPROC               glClearBufferiv;
extern PFNGLCLEARBUFFERUIVPROC              glClearBufferuiv;
extern PFNGLCLEARBUFFERFVPROC               glClearBufferfv;
extern PFNGLCLEARBUFFERFIPROC               glClearBufferfi;
extern PFNGLGETSTRINGIPROC                  glGetStringi;
extern PFNGLISRENDERBUFFERPROC              glIsRenderbuffer;
extern PFNGLBINDRENDERBUFFERPROC            glBindRenderbuffer;
extern PFNGLDELETERENDERBUFFERSPROC         glDeleteRenderbuffers;
extern PFNGLGENRENDERBUFFERSPROC            glGenRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEPROC         glRenderbufferStorage;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC  glGetRenderbufferParameteriv;
extern PFNGLISFRAMEBUFFERPROC               glIsFramebuffer;
extern PFNGLBINDFRAMEBUFFERPROC             glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC          glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC             glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC      glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC        glFramebufferTexture1D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC        glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC        glFramebufferTexture3D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC     glFramebufferRenderbuffer;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
extern PFNGLGENERATEMIPMAPPROC              glGenerateMipmap;
extern PFNGLBLITFRAMEBUFFERPROC             glBlitFramebuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC     glFramebufferTextureLayer;
extern PFNGLMAPBUFFERRANGEPROC              glMapBufferRange;
extern PFNGLFLUSHMAPPEDBUFFERRANGEPROC      glFlushMappedBufferRange;
extern PFNGLBINDVERTEXARRAYPROC             glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC          glDeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC             glGenVertexArrays;
extern PFNGLISVERTEXARRAYPROC               glIsVertexArray;

//GL_VERSION_3_1

extern PFNGLDRAWARRAYSINSTANCEDPROC        glDrawArraysInstanced;
extern PFNGLDRAWELEMENTSINSTANCEDPROC      glDrawElementsInstanced;
extern PFNGLTEXBUFFERPROC                  glTexBuffer;
extern PFNGLPRIMITIVERESTARTINDEXPROC      glPrimitiveRestartIndex;
extern PFNGLCOPYBUFFERSUBDATAPROC          glCopyBufferSubData;
extern PFNGLGETUNIFORMINDICESPROC          glGetUniformIndices;
extern PFNGLGETACTIVEUNIFORMSIVPROC        glGetActiveUniformsiv;
extern PFNGLGETACTIVEUNIFORMNAMEPROC       glGetActiveUniformName;
extern PFNGLGETUNIFORMBLOCKINDEXPROC       glGetUniformBlockIndex;
extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC    glGetActiveUniformBlockiv;
extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC  glGetActiveUniformBlockName;
extern PFNGLUNIFORMBLOCKBINDINGPROC        glUniformBlockBinding;

//GL_VERSION_3_2

extern PFNGLDRAWELEMENTSBASEVERTEXPROC          glDrawElementsBaseVertex;
extern PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC     glDrawRangeElementsBaseVertex;
extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
extern PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC     glMultiDrawElementsBaseVertex;
extern PFNGLPROVOKINGVERTEXPROC                 glProvokingVertex;
extern PFNGLFENCESYNCPROC                       glFenceSync;
extern PFNGLISSYNCPROC                          glIsSync;
extern PFNGLDELETESYNCPROC                      glDeleteSync;
extern PFNGLCLIENTWAITSYNCPROC                  glClientWaitSync;
extern PFNGLWAITSYNCPROC                        glWaitSync;
extern PFNGLGETINTEGER64VPROC                   glGetInteger64v;
extern PFNGLGETSYNCIVPROC                       glGetSynciv;
extern PFNGLGETINTEGER64I_VPROC                 glGetInteger64i_v;
extern PFNGLGETBUFFERPARAMETERI64VPROC          glGetBufferParameteri64v;
extern PFNGLFRAMEBUFFERTEXTUREPROC              glFramebufferTexture;
extern PFNGLTEXIMAGE2DMULTISAMPLEPROC           glTexImage2DMultisample;
extern PFNGLTEXIMAGE3DMULTISAMPLEPROC           glTexImage3DMultisample;
extern PFNGLGETMULTISAMPLEFVPROC                glGetMultisamplefv;
extern PFNGLSAMPLEMASKIPROC                     glSampleMaski;

//GL_VERSION_3_3

extern PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
extern PFNGLGETFRAGDATAINDEXPROC            glGetFragDataIndex;
extern PFNGLGENSAMPLERSPROC                 glGenSamplers;
extern PFNGLDELETESAMPLERSPROC              glDeleteSamplers;
extern PFNGLISSAMPLERPROC                   glIsSampler;
extern PFNGLBINDSAMPLERPROC                 glBindSampler;
extern PFNGLSAMPLERPARAMETERIPROC           glSamplerParameteri;
extern PFNGLSAMPLERPARAMETERIVPROC          glSamplerParameteriv;
extern PFNGLSAMPLERPARAMETERFPROC           glSamplerParameterf;
extern PFNGLSAMPLERPARAMETERFVPROC          glSamplerParameterfv;
extern PFNGLSAMPLERPARAMETERIIVPROC         glSamplerParameterIiv;
extern PFNGLSAMPLERPARAMETERIUIVPROC        glSamplerParameterIuiv;
extern PFNGLGETSAMPLERPARAMETERIVPROC       glGetSamplerParameteriv;
extern PFNGLGETSAMPLERPARAMETERIIVPROC      glGetSamplerParameterIiv;
extern PFNGLGETSAMPLERPARAMETERFVPROC       glGetSamplerParameterfv;
extern PFNGLGETSAMPLERPARAMETERIUIVPROC     glGetSamplerParameterIuiv;
extern PFNGLQUERYCOUNTERPROC                glQueryCounter;
extern PFNGLGETQUERYOBJECTI64VPROC          glGetQueryObjecti64v;
extern PFNGLGETQUERYOBJECTUI64VPROC         glGetQueryObjectui64v;
extern PFNGLVERTEXATTRIBDIVISORPROC         glVertexAttribDivisor;
/*extern  glVertexAttribP1ui;
extern  glVertexAttribP1uiv;
extern  glVertexAttribP2ui;
extern  glVertexAttribP2uiv;
extern  glVertexAttribP3ui;
extern  glVertexAttribP3uiv;
extern  glVertexAttribP4ui;
extern  glVertexAttribP4uiv;
extern  glVertexP2ui;
extern  glVertexP2uiv;
extern  glVertexP3ui;
extern  glVertexP3uiv;
extern  glVertexP4ui;
extern  glVertexP4uiv;
extern   glTexCoordP1ui;
extern   glTexCoordP1uiv;
extern   glTexCoordP2ui;
extern   glTexCoordP2uiv;
extern   glTexCoordP3ui;
extern   glTexCoordP3uiv;
extern   glTexCoordP4ui;
extern   glTexCoordP4uiv;
extern   glMultiTexCoordP1ui;
extern   glMultiTexCoordP1uiv;
extern   glMultiTexCoordP2ui;
extern   glMultiTexCoordP2uiv;
extern   glMultiTexCoordP3ui;
extern   glMultiTexCoordP3uiv;
extern   glMultiTexCoordP4ui;
extern   glMultiTexCoordP4uiv;
extern   glNormalP3ui;
extern   glNormalP3uiv;
extern   glColorP3ui;
extern   glColorP3uiv;
extern   glColorP4ui;
extern   glColorP4uiv;
extern   glSecondaryColorP3ui;
extern   glSecondaryColorP3uiv;

//GL_VERSION_4_0

extern APIENTRY glMinSampleShading;
extern APIENTRY glBlendEquationi;
extern APIENTRY glBlendEquationSeparatei;
extern APIENTRY glBlendFunci;
extern APIENTRY glBlendFuncSeparatei;
extern APIENTRY glDrawArraysIndirect;
extern APIENTRY glDrawElementsIndirect;
extern APIENTRY glUniform1d;
extern APIENTRY glUniform2d;
extern APIENTRY glUniform3d;
extern APIENTRY glUniform4d;
extern APIENTRY glUniform1dv;
extern APIENTRY glUniform2dv;
extern APIENTRY glUniform3dv;
extern APIENTRY glUniform4dv;
extern APIENTRY glUniformMatrix2dv;
extern APIENTRY glUniformMatrix3dv;
extern APIENTRY glUniformMatrix4dv;
extern APIENTRY glUniformMatrix2x3dv;
extern APIENTRY glUniformMatrix2x4dv;
extern APIENTRY glUniformMatrix3x2dv;
extern APIENTRY glUniformMatrix3x4dv;
extern APIENTRY glUniformMatrix4x2dv;
extern APIENTRY glUniformMatrix4x3dv;
extern APIENTRY glGetUniformdv;
extern APIENTRY glGetSubroutineUniformLocation;
extern APIENTRY glGetSubroutineIndex;
extern APIENTRY glGetActiveSubroutineUniformiv;
extern APIENTRY glGetActiveSubroutineUniformName;
extern APIENTRY glGetActiveSubroutineName;
extern APIENTRY glUniformSubroutinesuiv;
extern APIENTRY glGetUniformSubroutineuiv;
extern APIENTRY glGetProgramStageiv;
extern APIENTRY glPatchParameteri;
extern APIENTRY glPatchParameterfv;
extern APIENTRY glBindTransformFeedback;
extern APIENTRY glDeleteTransformFeedbacks;
extern APIENTRY glGenTransformFeedbacks;
extern APIENTRY glIsTransformFeedback;
extern APIENTRY glPauseTransformFeedback;
extern APIENTRY glResumeTransformFeedback;
extern APIENTRY glDrawTransformFeedback;
extern APIENTRY glDrawTransformFeedbackStream;
extern APIENTRY glBeginQueryIndexed;
extern APIENTRY glEndQueryIndexed;
extern APIENTRY glGetQueryIndexediv;

//GL_VERSION_4_1

extern APIENTRY glReleaseShaderCompiler;
extern APIENTRY glShaderBinary;
extern APIENTRY glGetShaderPrecisionFormat;
extern APIENTRY glDepthRangef;
extern APIENTRY glClearDepthf;
extern APIENTRY glGetProgramBinary;
extern APIENTRY glProgramBinary;
extern APIENTRY glProgramParameteri;
extern APIENTRY glUseProgramStages;
extern APIENTRY glActiveShaderProgram;
extern APIENTRY glCreateShaderProgramv;
extern APIENTRY glBindProgramPipeline;
extern APIENTRY glDeleteProgramPipelines;
extern APIENTRY glGenProgramPipelines;
extern APIENTRY glIsProgramPipeline;
extern APIENTRY glGetProgramPipelineiv;
extern APIENTRY glProgramUniform1i;
extern APIENTRY glProgramUniform1iv;
extern APIENTRY glProgramUniform1f;
extern APIENTRY glProgramUniform1fv;
extern APIENTRY glProgramUniform1d;
extern APIENTRY glProgramUniform1dv;
extern APIENTRY glProgramUniform1ui;
extern APIENTRY glProgramUniform1uiv;
extern APIENTRY glProgramUniform2i;
extern APIENTRY glProgramUniform2iv;
extern APIENTRY glProgramUniform2f;
extern APIENTRY glProgramUniform2fv;
extern APIENTRY glProgramUniform2d;
extern APIENTRY glProgramUniform2dv;
extern APIENTRY glProgramUniform2ui;
extern APIENTRY glProgramUniform2uiv;
extern APIENTRY glProgramUniform3i;
extern APIENTRY glProgramUniform3iv;
extern APIENTRY glProgramUniform3f;
extern APIENTRY glProgramUniform3fv
extern APIENTRY glProgramUniform3d;
extern APIENTRY glProgramUniform3dv;
extern APIENTRY glProgramUniform3ui;
extern APIENTRY glProgramUniform3uiv;
extern APIENTRY glProgramUniform4i;
extern APIENTRY glProgramUniform4iv;
extern APIENTRY glProgramUniform4f;
extern APIENTRY glProgramUniform4fv;
extern APIENTRY glProgramUniform4d;
extern APIENTRY glProgramUniform4dv;
extern APIENTRY glProgramUniform4ui;
extern APIENTRY glProgramUniform4uiv;
extern APIENTRY glProgramUniformMatrix2fv;
extern APIENTRY glProgramUniformMatrix3fv;
extern APIENTRY glProgramUniformMatrix4fv;
extern APIENTRY glProgramUniformMatrix2dv;
extern APIENTRY glProgramUniformMatrix3dv;
extern APIENTRY glProgramUniformMatrix4dv;
extern APIENTRY glProgramUniformMatrix2x3fv;
extern APIENTRY glProgramUniformMatrix3x2fv;
extern APIENTRY glProgramUniformMatrix2x4fv;
extern APIENTRY glProgramUniformMatrix4x2fv;
extern APIENTRY glProgramUniformMatrix3x4fv;
extern APIENTRY glProgramUniformMatrix4x3fv;
extern APIENTRY glProgramUniformMatrix2x3dv;
extern APIENTRY glProgramUniformMatrix3x2dv;
extern APIENTRY glProgramUniformMatrix2x4dv;
extern APIENTRY glProgramUniformMatrix4x2dv;
extern APIENTRY glProgramUniformMatrix3x4dv;
extern APIENTRY glProgramUniformMatrix4x3dv;
extern APIENTRY glValidateProgramPipeline;
extern APIENTRY glGetProgramPipelineInfoLog;
extern APIENTRY glVertexAttribL1d;
extern APIENTRY glVertexAttribL2d;
extern APIENTRY glVertexAttribL3d;
extern APIENTRY glVertexAttribL4d;
extern APIENTRY glVertexAttribL1dv;
extern APIENTRY glVertexAttribL2dv;
extern APIENTRY glVertexAttribL3dv;
extern APIENTRY glVertexAttribL4dv;
extern APIENTRY glVertexAttribLPointer;
extern APIENTRY glGetVertexAttribLdv;
extern APIENTRY glViewportArrayv;
extern APIENTRY glViewportIndexedf;
extern APIENTRY glViewportIndexedfv;
extern APIENTRY glScissorArrayv;
extern APIENTRY glScissorIndexed;
extern APIENTRY glScissorIndexedv;
extern APIENTRY glDepthRangeArrayv;
extern APIENTRY glDepthRangeIndexed;
extern APIENTRY glGetFloati_v;
extern APIENTRY glGetDoublei_v;
*/
//GL_VERSION_4_2

extern PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC             glDrawArraysInstancedBaseInstance;
extern PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC           glDrawElementsInstancedBaseInstance;
extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;
extern PFNGLGETINTERNALFORMATIVPROC                         glGetInternalformativ;
extern PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC              glGetActiveAtomicCounterBufferiv;
extern PFNGLBINDIMAGETEXTUREPROC                            glBindImageTexture;
extern PFNGLMEMORYBARRIERPROC                               glMemoryBarrier;
extern PFNGLTEXSTORAGE1DPROC                                glTexStorage1D;
extern PFNGLTEXSTORAGE2DPROC                                glTexStorage2D;
extern PFNGLTEXSTORAGE3DPROC                                glTexStorage3D;
extern PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC              glDrawTransformFeedbackInstanced;
extern PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC        glDrawTransformFeedbackStreamInstanced;

//GL_VERSION_4_3

extern PFNGLCLEARBUFFERDATAPROC                 glClearBufferData;
extern PFNGLCLEARBUFFERSUBDATAPROC              glClearBufferSubData;
extern PFNGLDISPATCHCOMPUTEPROC                 glDispatchCompute;
extern PFNGLDISPATCHCOMPUTEINDIRECTPROC         glDispatchComputeIndirect;
extern PFNGLCOPYIMAGESUBDATAPROC                glCopyImageSubData;
extern PFNGLFRAMEBUFFERPARAMETERIPROC           glFramebufferParameteri;
extern PFNGLGETFRAMEBUFFERPARAMETERIVPROC       glGetFramebufferParameteriv;
extern PFNGLGETINTERNALFORMATI64VPROC           glGetInternalformati64v;
extern PFNGLINVALIDATETEXSUBIMAGEPROC           glInvalidateTexSubImage;
extern PFNGLINVALIDATETEXIMAGEPROC              glInvalidateTexImage;
extern PFNGLINVALIDATEBUFFERSUBDATAPROC         glInvalidateBufferSubData;
extern PFNGLINVALIDATEBUFFERDATAPROC            glInvalidateBufferData;
extern PFNGLINVALIDATEFRAMEBUFFERPROC           glInvalidateFramebuffer;
extern PFNGLINVALIDATESUBFRAMEBUFFERPROC        glInvalidateSubFramebuffer;
extern PFNGLMULTIDRAWARRAYSINDIRECTPROC         glMultiDrawArraysIndirect;
extern PFNGLMULTIDRAWELEMENTSINDIRECTPROC       glMultiDrawElementsIndirect;
extern PFNGLGETPROGRAMINTERFACEIVPROC           glGetProgramInterfaceiv;
extern PFNGLGETPROGRAMRESOURCEINDEXPROC         glGetProgramResourceIndex;
extern PFNGLGETPROGRAMRESOURCENAMEPROC          glGetProgramResourceName;
extern PFNGLGETPROGRAMRESOURCEIVPROC            glGetProgramResourceiv;
extern PFNGLGETPROGRAMRESOURCELOCATIONPROC      glGetProgramResourceLocation;
extern PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glGetProgramResourceLocationIndex;
extern PFNGLSHADERSTORAGEBLOCKBINDINGPROC       glShaderStorageBlockBinding;
extern PFNGLTEXBUFFERRANGEPROC                  glTexBufferRange;
extern PFNGLTEXSTORAGE2DMULTISAMPLEPROC         glTexStorage2DMultisample;
extern PFNGLTEXSTORAGE3DMULTISAMPLEPROC         glTexStorage3DMultisample;
extern PFNGLTEXTUREVIEWPROC                     glTextureView;
extern PFNGLBINDVERTEXBUFFERPROC                glBindVertexBuffer;
extern PFNGLVERTEXATTRIBFORMATPROC              glVertexAttribFormat;
extern PFNGLVERTEXATTRIBIFORMATPROC             glVertexAttribIFormat;
extern PFNGLVERTEXATTRIBLFORMATPROC             glVertexAttribLFormat;
extern PFNGLVERTEXATTRIBBINDINGPROC             glVertexAttribBinding;
extern PFNGLVERTEXBINDINGDIVISORPROC            glVertexBindingDivisor;
extern PFNGLDEBUGMESSAGECONTROLPROC             glDebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTPROC              glDebugMessageInsert;
extern PFNGLDEBUGMESSAGECALLBACKPROC            glDebugMessageCallback;
extern PFNGLGETDEBUGMESSAGELOGPROC              glGetDebugMessageLog;
extern PFNGLPUSHDEBUGGROUPPROC                  glPushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC                   glPopDebugGroup;
extern PFNGLOBJECTLABELPROC                     glObjectLabel;
extern PFNGLGETOBJECTLABELPROC                  glGetObjectLabel;
extern PFNGLOBJECTPTRLABELPROC                  glObjectPtrLabel;
extern PFNGLGETOBJECTPTRLABELPROC               glGetObjectPtrLabel;
