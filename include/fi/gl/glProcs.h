#ifndef _GL2FUNCS_H
#define _GL2FUNCS_H

#include "fi/gl/glcorearb.h"

#ifdef LINUX
	#include <GL/glx.h>
	#define getProcAddress(x) glXGetProcAddress((const GLubyte*)x)
#elif defined(OSX) || defined(__ANDROID__)

	#if defined(OSX)
		#ifdef __cplusplus
		extern "C" {
		#endif
			void* getProcAddress(const char *proc);
		#ifdef __cplusplus
		}
		#endif
	#elif defined(__ANDROID__)
		#include <EGL/egl.h>
		#define getProcAddress(x) eglGetProcAddress((char const *)x)
	#endif

	// GL 1.X
	extern PFNGLGETSTRINGPROC glGetString;
	extern PFNGLGETINTEGERVPROC glGetIntegerv;
	extern PFNGLFLUSHPROC glFlush;
	extern PFNGLFINISHPROC glFinish;
	extern PFNGLENABLEPROC glEnable;
	extern PFNGLDISABLEPROC glDisable;
	extern PFNGLCLEARPROC glClear;
	extern PFNGLCLEARCOLORPROC glClearColor;
	extern PFNGLDRAWARRAYSPROC glDrawArrays;
	extern PFNGLDRAWELEMENTSPROC glDrawElements;
	extern PFNGLVIEWPORTPROC glViewport;
	extern PFNGLSCISSORPROC glScissor;
	extern PFNGLGETERRORPROC glGetError;

	extern PFNGLTEXIMAGE2DPROC glTexImage2D;
	extern PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
	extern PFNGLDELETETEXTURESPROC glDeleteTextures;
	extern PFNGLACTIVETEXTUREPROC glActiveTexture;
	extern PFNGLTEXPARAMETERIPROC glTexParameteri;
	extern PFNGLTEXPARAMETERFPROC glTexParameterf;
	extern PFNGLBINDTEXTUREPROC glBindTexture;
	extern PFNGLGENTEXTURESPROC glGenTextures;

	extern PFNGLCULLFACEPROC glCullFace;
	extern PFNGLFRONTFACEPROC glFrontFace;
	extern PFNGLBLENDFUNCPROC glBlendFunc;

	extern PFNGLREADPIXELSPROC glReadPixels;
	extern PFNGLPIXELSTOREIPROC glPixelStorei;
	extern PFNGLPIXELSTOREFPROC glPixelStoref;

#endif

#ifdef __cplusplus
extern "C" {
#endif

	void open_libgl(void);
	void close_libgl(void);

#ifdef __cplusplus
}
#endif


// GL 1.5
extern PFNGLGENQUERIESPROC glGenQueries;
extern PFNGLDELETEQUERIESPROC glDeleteQueries;
extern PFNGLISQUERYPROC glIsQuery;
extern PFNGLBEGINQUERYPROC glBeginQuery;
extern PFNGLENDQUERYPROC glEndQuery;
extern PFNGLGETQUERYIVPROC glGetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLISBUFFERPROC glIsBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
extern PFNGLMAPBUFFERPROC glMapBuffer;
extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;

// GL 2.0
extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
extern PFNGLDRAWBUFFERSPROC glDrawBuffers;
extern PFNGLSTENCILOPSEPARATEPROC glStencilSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLDETACHSHADERPROC glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC glGetAttachedShader;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC glIsProgram;
extern PFNGLISSHADERPROC glIsShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM2IPROC glUniform2i;
extern PFNGLUNIFORM3IPROC glUniform3i;
extern PFNGLUNIFORM4IPROC glUniform4i;
extern PFNGLUNIFORM1FVPROC glUniform1fv;
extern PFNGLUNIFORM2FVPROC glUniform2fv;
extern PFNGLUNIFORM3FVPROC glUniform3fv;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUNIFORM1IVPROC glUniform1iv;
extern PFNGLUNIFORM2IVPROC glUniform2iv;
extern PFNGLUNIFORM3IVPROC glUniform3iv;
extern PFNGLUNIFORM4IVPROC glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
extern PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
extern PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
extern PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
extern PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
extern PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
extern PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
extern PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
extern PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
extern PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
extern PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
extern PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4SVPROC glvertexAttrib4sv;
extern PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

// GL 2.1
extern PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

// GL 3.0
extern PFNGLCOLORMASKIPROC glColorMaski;
extern PFNGLGETBOOLEANI_VPROC glGetBooleani_v;
extern PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
extern PFNGLENABLEIPROC glEnablei;
extern PFNGLDISABLEIPROC glDisablei;
extern PFNGLISENABLEDIPROC glIsEnabledi;
extern PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
extern PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
extern PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
extern PFNGLBINDBUFFERBASEPROC glBindBufferBase;
extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
extern PFNGLCLAMPCOLORPROC glClampColor;
extern PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender;
extern PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender;
extern PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
extern PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv;
extern PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv;
extern PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
extern PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;
extern PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;
extern PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;
/*
extern PFNGLVERTEXATTRIBI1UIPROC (GLuint index, GLuint x);
extern PFNGLVERTEXATTRIBI2UIPROC (GLuint index, GLuint x, GLuint y);
extern PFNGLVERTEXATTRIBI3UIPROC (GLuint index, GLuint x, GLuint y, GLuint z);
extern PFNGLVERTEXATTRIBI4UIPROC (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
extern PFNGLVERTEXATTRIBI1IVPROC (GLuint index, const GLint *v);
extern PFNGLVERTEXATTRIBI2IVPROC (GLuint index, const GLint *v);
extern PFNGLVERTEXATTRIBI3IVPROC (GLuint index, const GLint *v);
extern PFNGLVERTEXATTRIBI4IVPROC (GLuint index, const GLint *v);
extern PFNGLVERTEXATTRIBI1UIVPROC (GLuint index, const GLuint *v);
extern PFNGLVERTEXATTRIBI2UIVPROC (GLuint index, const GLuint *v);
extern PFNGLVERTEXATTRIBI3UIVPROC (GLuint index, const GLuint *v);
extern PFNGLVERTEXATTRIBI4UIVPROC (GLuint index, const GLuint *v);
extern PFNGLVERTEXATTRIBI4BVPROC (GLuint index, const GLbyte *v);
extern PFNGLVERTEXATTRIBI4SVPROC (GLuint index, const GLshort *v);
extern PFNGLVERTEXATTRIBI4UBVPROC (GLuint index, const GLubyte *v);
extern PFNGLVERTEXATTRIBI4USVPROC (GLuint index, const GLushort *v);
extern PFNGLGETUNIFORMUIVPROC (GLuint program, GLint location, GLuint *params);
extern PFNGLBINDFRAGDATALOCATIONPROC (GLuint program, GLuint color, const GLchar *name);
extern PFNGLGETFRAGDATALOCATIONPROC (GLuint program, const GLchar *name);
extern PFNGLUNIFORM1UIPROC (GLint location, GLuint v0);
extern PFNGLUNIFORM2UIPROC (GLint location, GLuint v0, GLuint v1);
extern PFNGLUNIFORM3UIPROC (GLint location, GLuint v0, GLuint v1, GLuint v2);
extern PFNGLUNIFORM4UIPROC (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
extern PFNGLUNIFORM1UIVPROC (GLint location, GLsizei count, const GLuint *value);
extern PFNGLUNIFORM2UIVPROC (GLint location, GLsizei count, const GLuint *value);
extern PFNGLUNIFORM3UIVPROC (GLint location, GLsizei count, const GLuint *value);
extern PFNGLUNIFORM4UIVPROC (GLint location, GLsizei count, const GLuint *value);

extern PFNGLTEXPARAMETERIIVPROC (GLenum target, GLenum pname, const GLint *params);
extern PFNGLTEXPARAMETERIUIVPROC (GLenum target, GLenum pname, const GLuint *params);
extern PFNGLGETTEXPARAMETERIIVPROC (GLenum target, GLenum pname, GLint *params);
extern PFNGLGETTEXPARAMETERIUIVPROC (GLenum target, GLenum pname, GLuint *params);
extern PFNGLCLEARBUFFERIVPROC (GLenum buffer, GLint drawbuffer, const GLint *value);
extern PFNGLCLEARBUFFERUIVPROC (GLenum buffer, GLint drawbuffer, const GLuint *value);
extern PFNGLCLEARBUFFERFVPROC (GLenum buffer, GLint drawbuffer, const GLfloat *value);
extern PFNGLCLEARBUFFERFIPROC (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
*/
extern PFNGLGETSTRINGIPROC glGetStringi;
extern PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
extern PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
extern PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
extern PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLISVERTEXARRAYPROC glIsVertexArray;

// GL 3.1
extern PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
extern PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
extern PFNGLTEXBUFFERPROC glTexBuffer;
extern PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;
extern PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
extern PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
extern PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
extern PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
extern PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
extern PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;

// GL 3.2
extern PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
extern PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
extern PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;
extern PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
extern PFNGLFENCESYNCPROC glFenceSync;
extern PFNGLISSYNCPROC glIsSync;
extern PFNGLDELETESYNCPROC glDeleteSync;
extern PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
extern PFNGLWAITSYNCPROC glWaitSync;
extern PFNGLGETINTEGER64VPROC glGetInteger64v;
extern PFNGLGETSYNCIVPROC glGetSynciv;
extern PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
extern PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
extern PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
extern PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
extern PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
extern PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
extern PFNGLSAMPLEMASKIPROC glSampleMaski;

#ifdef __cplusplus
extern "C" {
#endif

	void initGL1XFuncs();
	void initGL10Funcs();
	void initGL15Funcs();
	void initGL20Funcs();
	void initGL21Funcs();
	void initGL30Funcs();
	void initGL31Funcs();
	void initGL32Funcs();

#ifdef __cplusplus
}
#endif


#endif //_GL2FUNCS_H
