#include <GL/glProcs.h>

#if defined(OSX) || defined(__ANDROID__)
	#if defined(OSX)
		#include <Carbon/Carbon.h>

		CFBundleRef __bundle;
		CFURLRef __bundleURL;

		void open_libgl(void)
		{
			__bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
					CFSTR("/System/Library/Frameworks/OpenGL.framework"),
						kCFURLPOSIXPathStyle, true);

			__bundle = CFBundleCreate(kCFAllocatorDefault, __bundleURL);
			assert(__bundle != NULL);
		}

		void close_libgl(void)
		{
			CFRelease(__bundle);
			CFRelease(__bundleURL);
		}

		void* getProcAddress(const char *proc)
		{
			void* res;

			CFStringRef procname = CFStringCreateWithCString(kCFAllocatorDefault, proc, kCFStringEncodingASCII);
			res = (void*) CFBundleGetFunctionPointerForName(__bundle, procname);
			CFRelease(procname);
			if(!res)
			{
				printf("GL procedure[ %s ] not found.\n", proc);
			}
			return res;
		}
	#else
		void open_libgl(){}
		void close_libgl(){}
	#endif // OSX


// GL 1.X
PFNGLGETSTRINGPROC glGetString;
PFNGLGETINTEGERVPROC glGetIntegerv;
PFNGLFLUSHPROC glFlush;
PFNGLFINISHPROC glFinish;
PFNGLENABLEPROC glEnable;
PFNGLDISABLEPROC glDisable;
PFNGLCLEARPROC glClear;
PFNGLCLEARCOLORPROC glClearColor;
PFNGLDRAWARRAYSPROC glDrawArrays;
PFNGLDRAWELEMENTSPROC glDrawElements;
PFNGLVIEWPORTPROC glViewport;
PFNGLSCISSORPROC glScissor;
PFNGLGETERRORPROC glGetError;

PFNGLTEXIMAGE2DPROC glTexImage2D;
PFNGLDELETETEXTURESPROC glDeleteTextures;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLTEXPARAMETERIPROC glTexParameteri;
PFNGLTEXPARAMETERFPROC glTexParameterf;
PFNGLBINDTEXTUREPROC glBindTexture;
PFNGLGENTEXTURESPROC glGenTextures;

PFNGLCULLFACEPROC glCullFace;
PFNGLFRONTFACEPROC glFrontFace;
PFNGLBLENDFUNCPROC glBlendFunc;

PFNGLREADPIXELSPROC glReadPixels;
PFNGLPIXELSTOREIPROC glPixelStorei;
PFNGLPIXELSTOREFPROC glPixelStoref;

#else
void open_libgl(void)
{
}

void close_libgl(void)
{
}
#endif /*OSX*/

// GL 1.5
PFNGLGENQUERIESPROC glGenQueries;
PFNGLDELETEQUERIESPROC glDeleteQueries;
PFNGLISQUERYPROC glIsQuery;
PFNGLBEGINQUERYPROC glBeginQuery;
PFNGLENDQUERYPROC glEndQuery;
PFNGLGETQUERYIVPROC glGetQueryiv;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLISBUFFERPROC glIsBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;

// GL 2.0
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
PFNGLDRAWBUFFERSPROC glDrawBuffers;
PFNGLSTENCILOPSEPARATEPROC glStencilSeparate;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShader;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERSOURCEPROC glGetShaderSource;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETUNIFORMFVPROC glGetUniformfv;
PFNGLGETUNIFORMIVPROC glGetUniformiv;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
PFNGLISPROGRAMPROC glIsProgram;
PFNGLISSHADERPROC glIsShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORM2IVPROC glUniform2iv;
PFNGLUNIFORM3IVPROC glUniform3iv;
PFNGLUNIFORM4IVPROC glUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
PFNGLVERTEXATTRIB4SVPROC glvertexAttrib4sv;
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

// GL 2.1
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

// GL 3.0
PFNGLCOLORMASKIPROC glColorMaski;
PFNGLGETBOOLEANI_VPROC glGetBooleani_v;
PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
PFNGLENABLEIPROC glEnablei;
PFNGLDISABLEIPROC glDisablei;
PFNGLISENABLEDIPROC glIsEnabledi;
PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
PFNGLCLAMPCOLORPROC glClampColor;
PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender;
PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv;
PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv;
PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;
PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;
PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;
/*
PFNGLVERTEXATTRIBI1UIPROC (GLuint index, GLuint x);
PFNGLVERTEXATTRIBI2UIPROC (GLuint index, GLuint x, GLuint y);
PFNGLVERTEXATTRIBI3UIPROC (GLuint index, GLuint x, GLuint y, GLuint z);
PFNGLVERTEXATTRIBI4UIPROC (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
PFNGLVERTEXATTRIBI1IVPROC (GLuint index, const GLint *v);
PFNGLVERTEXATTRIBI2IVPROC (GLuint index, const GLint *v);
PFNGLVERTEXATTRIBI3IVPROC (GLuint index, const GLint *v);
PFNGLVERTEXATTRIBI4IVPROC (GLuint index, const GLint *v);
PFNGLVERTEXATTRIBI1UIVPROC (GLuint index, const GLuint *v);
PFNGLVERTEXATTRIBI2UIVPROC (GLuint index, const GLuint *v);
PFNGLVERTEXATTRIBI3UIVPROC (GLuint index, const GLuint *v);
PFNGLVERTEXATTRIBI4UIVPROC (GLuint index, const GLuint *v);
PFNGLVERTEXATTRIBI4BVPROC (GLuint index, const GLbyte *v);
PFNGLVERTEXATTRIBI4SVPROC (GLuint index, const GLshort *v);
PFNGLVERTEXATTRIBI4UBVPROC (GLuint index, const GLubyte *v);
PFNGLVERTEXATTRIBI4USVPROC (GLuint index, const GLushort *v);
PFNGLGETUNIFORMUIVPROC (GLuint program, GLint location, GLuint *params);
PFNGLBINDFRAGDATALOCATIONPROC (GLuint program, GLuint color, const GLchar *name);
PFNGLGETFRAGDATALOCATIONPROC (GLuint program, const GLchar *name);
PFNGLUNIFORM1UIPROC (GLint location, GLuint v0);
PFNGLUNIFORM2UIPROC (GLint location, GLuint v0, GLuint v1);
PFNGLUNIFORM3UIPROC (GLint location, GLuint v0, GLuint v1, GLuint v2);
PFNGLUNIFORM4UIPROC (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
PFNGLUNIFORM1UIVPROC (GLint location, GLsizei count, const GLuint *value);
PFNGLUNIFORM2UIVPROC (GLint location, GLsizei count, const GLuint *value);
PFNGLUNIFORM3UIVPROC (GLint location, GLsizei count, const GLuint *value);
PFNGLUNIFORM4UIVPROC (GLint location, GLsizei count, const GLuint *value);

PFNGLTEXPARAMETERIIVPROC (GLenum target, GLenum pname, const GLint *params);
PFNGLTEXPARAMETERIUIVPROC (GLenum target, GLenum pname, const GLuint *params);
PFNGLGETTEXPARAMETERIIVPROC (GLenum target, GLenum pname, GLint *params);
PFNGLGETTEXPARAMETERIUIVPROC (GLenum target, GLenum pname, GLuint *params);
PFNGLCLEARBUFFERIVPROC (GLenum buffer, GLint drawbuffer, const GLint *value);
PFNGLCLEARBUFFERUIVPROC (GLenum buffer, GLint drawbuffer, const GLuint *value);
PFNGLCLEARBUFFERFVPROC (GLenum buffer, GLint drawbuffer, const GLfloat *value);
PFNGLCLEARBUFFERFIPROC (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
*/
PFNGLGETSTRINGIPROC glGetStringi;
PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLISVERTEXARRAYPROC glIsVertexArray;

// GL 3.1
PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
PFNGLTEXBUFFERPROC glTexBuffer;
PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;
PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;

// GL 3.2
PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;
PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
PFNGLFENCESYNCPROC glFenceSync;
PFNGLISSYNCPROC glIsSync;
PFNGLDELETESYNCPROC glDeleteSync;
PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
PFNGLWAITSYNCPROC glWaitSync;
PFNGLGETINTEGER64VPROC glGetInteger64v;
PFNGLGETSYNCIVPROC glGetSynciv;
PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
PFNGLSAMPLEMASKIPROC glSampleMaski;


void initGL10Funcs()
{
#if defined(OSX) || defined(__ANDROID__)
	glGetString = (PFNGLGETSTRINGPROC)getProcAddress("glGetString");
	glGetIntegerv = (PFNGLGETINTEGERVPROC)getProcAddress("glGetIntegerv");
	glFlush = (PFNGLFLUSHPROC)getProcAddress("glFlush");
	glFinish = (PFNGLFINISHPROC)getProcAddress("glFinish");
	glEnable = (PFNGLENABLEPROC)getProcAddress("glEnable");
	glDisable = (PFNGLDISABLEPROC)getProcAddress("glDisable");
	glClear = (PFNGLCLEARPROC)getProcAddress("glClear");
	glClearColor = (PFNGLCLEARCOLORPROC)getProcAddress("glClearColor");
	glDrawArrays = (PFNGLDRAWARRAYSPROC)getProcAddress("glDrawArrays");
	glDrawElements = (PFNGLDRAWELEMENTSPROC)getProcAddress("glDrawElements");
	glViewport = (PFNGLVIEWPORTPROC)getProcAddress("glViewport");
	glScissor = (PFNGLSCISSORPROC)getProcAddress("glScissor");
	glGetError = (PFNGLGETERRORPROC)getProcAddress("glGetError");

	glTexImage2D = (PFNGLTEXIMAGE2DPROC)getProcAddress("glTexImage2D");
	glDeleteTextures = (PFNGLDELETETEXTURESPROC)getProcAddress("glDeleteTextures");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)getProcAddress("glActiveTexture");
	glTexParameteri = (PFNGLTEXPARAMETERIPROC)getProcAddress("glTexParameteri");
	glTexParameterf = (PFNGLTEXPARAMETERFPROC)getProcAddress("glTexParameterf");
	glBindTexture = (PFNGLBINDTEXTUREPROC)getProcAddress("glBindTexture");
	glGenTextures = (PFNGLGENTEXTURESPROC)getProcAddress("glGenTextures");

	glCullFace = (PFNGLCULLFACEPROC)getProcAddress("glCullFace");
	glFrontFace = (PFNGLFRONTFACEPROC)getProcAddress("glFrontFace");
	glBlendFunc = (PFNGLBLENDFUNCPROC)getProcAddress("glBlendFunc");
	
	glReadPixels = (PFNGLREADPIXELSPROC)getProcAddress("glReadPixels");
	glPixelStorei = (PFNGLPIXELSTOREIPROC)getProcAddress("glPixelStorei");
	glPixelStoref = (PFNGLPIXELSTOREFPROC)getProcAddress("glPixelStoref");
#endif
}

void initGL15Funcs()
{
	glGenQueries = (PFNGLGENQUERIESPROC)getProcAddress("glGenQueries");
	glDeleteQueries = (PFNGLDELETEQUERIESPROC)getProcAddress("glDeleteQueries");
	glIsQuery = (PFNGLISQUERYPROC)getProcAddress("glIsQuery");
	glBeginQuery = (PFNGLBEGINQUERYPROC)getProcAddress("glBeginQuery");
	glEndQuery = (PFNGLENDQUERYPROC)getProcAddress("glEndQuery");
	glGetQueryiv = (PFNGLGETQUERYIVPROC)getProcAddress("glGetQueryiv");
	glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)getProcAddress("glGetQueryObjectiv");
	glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)getProcAddress("glGetQueryObjectuiv");
	glBindBuffer = (PFNGLBINDBUFFERPROC)getProcAddress("glBindBuffer");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)getProcAddress("glDeleteBuffers");
	glGenBuffers = (PFNGLGENBUFFERSPROC)getProcAddress("glGenBuffers");
	glIsBuffer = (PFNGLISBUFFERPROC)getProcAddress("glIsBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)getProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)getProcAddress("glBufferSubData");
	glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)getProcAddress("glGetBufferSubData");
	glMapBuffer = (PFNGLMAPBUFFERPROC)getProcAddress("glMapBuffer");
	glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)getProcAddress("glUnmapBuffer");
	glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)getProcAddress("glGetBufferParameteriv");
	glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)getProcAddress("glGetBufferPointerv");
}

void initGL1XFuncs()
{
	initGL10Funcs();
	initGL15Funcs();
}

void initGL20Funcs()
{
	glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)getProcAddress("glBlendEquationSeparate");
	glDrawBuffers = (PFNGLDRAWBUFFERSPROC)getProcAddress("glDrawBuffers");
	glStencilSeparate = (PFNGLSTENCILOPSEPARATEPROC)getProcAddress("glStencilSeparate");
	glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)getProcAddress("glStencilFuncSeparate");
	glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)getProcAddress("glStencilMaskSeparate");
	glAttachShader = (PFNGLATTACHSHADERPROC)getProcAddress("glAttachShader");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)getProcAddress("glBindAttribLocation");
	glCompileShader = (PFNGLCOMPILESHADERPROC)getProcAddress("glCompileShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)getProcAddress("glCreateProgram");
	glCreateShader = (PFNGLCREATESHADERPROC)getProcAddress("glCreateShader");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)getProcAddress("glDeleteProgram");
	glDeleteShader = (PFNGLDELETESHADERPROC)getProcAddress("glDeleteShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)getProcAddress("glDetachShader");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)getProcAddress("glDisableVertexAttribArray");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)getProcAddress("glEnableVertexAttribArray");
	glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)getProcAddress("glGetActiveAttrib");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)getProcAddress("glGetActiveUniform");
	glGetAttachedShader = (PFNGLGETATTACHEDSHADERSPROC)getProcAddress("glGetAttachedShader");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)getProcAddress("glGetAttribLocation");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)getProcAddress("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)getProcAddress("glGetProgramInfoLog");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)getProcAddress("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)getProcAddress("glGetShaderInfoLog");
	glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)getProcAddress("glGetShaderSource");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)getProcAddress("glGetUniformLocation");
	glGetUniformfv = (PFNGLGETUNIFORMFVPROC)getProcAddress("glGetUniformfv");
	glGetUniformiv = (PFNGLGETUNIFORMIVPROC)getProcAddress("glGetUniformiv");
	glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)getProcAddress("glGetVertexAttribdv");
	glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)getProcAddress("glGetVertexAttribfv");
	glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)getProcAddress("glGetVertexAttribiv");
	glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)getProcAddress("glGetVertexAttribPointerv");
	glIsProgram = (PFNGLISPROGRAMPROC)getProcAddress("glIsProgram");
	glIsShader = (PFNGLISSHADERPROC)getProcAddress("glIsShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)getProcAddress("glLinkProgram");
	glShaderSource = (PFNGLSHADERSOURCEPROC)getProcAddress("glShaderSource");
	glUseProgram = (PFNGLUSEPROGRAMPROC)getProcAddress("glUseProgram");
	glUniform1f = (PFNGLUNIFORM1FPROC)getProcAddress("glUniform1f");
	glUniform2f = (PFNGLUNIFORM2FPROC)getProcAddress("glUniform2f");
	glUniform3f = (PFNGLUNIFORM3FPROC)getProcAddress("glUniform3f");
	glUniform4f = (PFNGLUNIFORM4FPROC)getProcAddress("glUniform4f");
	glUniform1i = (PFNGLUNIFORM1IPROC)getProcAddress("glUniform1i");
	glUniform2i = (PFNGLUNIFORM2IPROC)getProcAddress("glUniform2i");
	glUniform3i = (PFNGLUNIFORM3IPROC)getProcAddress("glUniform3i");
	glUniform4i = (PFNGLUNIFORM4IPROC)getProcAddress("glUniform4i");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)getProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)getProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)getProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)getProcAddress("glUniform4fv");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)getProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)getProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)getProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)getProcAddress("glUniform4iv");
	glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)getProcAddress("glUniformMatrix2fv");
	glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)getProcAddress("glUniformMatrix3fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)getProcAddress("glUniformMatrix4fv");
	glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)getProcAddress("glValidateProgram");
	glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)getProcAddress("glVertexAttrib1d");
	glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)getProcAddress("glVertexAttrib1dv");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)getProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)getProcAddress("glVertexAttrib1fv");
	glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)getProcAddress("glVertexAttrib1s");
	glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)getProcAddress("glVertexAttrib1sv");
	glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)getProcAddress("glVertexAttrib2d");
	glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)getProcAddress("glVertexAttrib2dv");
	glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)getProcAddress("glVertexAttrib2f");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)getProcAddress("glVertexAttrib2fv");
	glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)getProcAddress("glVertexAttrib2s");
	glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)getProcAddress("glVertexAttrib2sv");
	glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)getProcAddress("glVertexAttrib3d");
	glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)getProcAddress("glVertexAttrib3dv");
	glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)getProcAddress("glVertexAttrib3f");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)getProcAddress("glVertexAttrib3fv");
	glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)getProcAddress("glVertexAttrib3s");
	glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)getProcAddress("glVertexAttrib3sv");
	glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)getProcAddress("glVertexAttrib4Nbv");
	glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)getProcAddress("glVertexAttrib4Niv");
	glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)getProcAddress("glVertexAttrib4Nsv");
	glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)getProcAddress("glVertexAttrib4Nub");
	glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)getProcAddress("glVertexAttrib4Nubv");
	glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)getProcAddress("glVertexAttrib4Nuiv");
	glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)getProcAddress("glVertexAttrib4Nusv");
	glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)getProcAddress("glVertexAttrib4bv");
	glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)getProcAddress("glVertexAttrib4d");
	glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)getProcAddress("glVertexAttrib4dv");
	glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)getProcAddress("glVertexAttrib4f");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)getProcAddress("glVertexAttrib4fv");
	glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)getProcAddress("glVertexAttrib4iv");
	glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)getProcAddress("glVertexAttrib4s");
	glvertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)getProcAddress("glvertexAttrib4sv");
	glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)getProcAddress("glVertexAttrib4ubv");
	glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)getProcAddress("glVertexAttrib4uiv");
	glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)getProcAddress("glVertexAttrib4usv");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)getProcAddress("glVertexAttribPointer");
}

void initGL21Funcs()
{
	glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)getProcAddress("glUniformMatrix2x3fv");
	glUniformMatrix3x2fv=(PFNGLUNIFORMMATRIX3X2FVPROC)getProcAddress("glUniformMatrix3x2fv");
	glUniformMatrix2x4fv=(PFNGLUNIFORMMATRIX2X4FVPROC)getProcAddress("glUniformMatrix2x4fv");
	glUniformMatrix4x2fv=(PFNGLUNIFORMMATRIX4X2FVPROC)getProcAddress("glUniformMatrix4x2fv");
	glUniformMatrix3x4fv=(PFNGLUNIFORMMATRIX3X4FVPROC)getProcAddress("glUniformMatrix3x4fv");
	glUniformMatrix4x3fv=(PFNGLUNIFORMMATRIX4X3FVPROC)getProcAddress("glUniformMatrix4x3fv");
}

void initGL30Funcs()
{
	glColorMaski = (PFNGLCOLORMASKIPROC)getProcAddress("glColorMaski");
	glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)getProcAddress("glGetBooleani_v");
	glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)getProcAddress("glGetIntegeri_v");
	glEnablei = (PFNGLENABLEIPROC)getProcAddress("glEnablei");
	glDisablei = (PFNGLDISABLEIPROC)getProcAddress("glDisablei");
	glIsEnabledi = (PFNGLISENABLEDIPROC)getProcAddress("glIsEnabledi");
	glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)getProcAddress("glBeginTransformFeedback");
	glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)getProcAddress("glEndTransformFeedback");
	glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)getProcAddress("glBindBufferRange");
	glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)getProcAddress("glBindBufferBase");
	glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)getProcAddress("glTransformFeedbackVaryings");
	glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)getProcAddress("glGetTransformFeedbackVarying");
	glClampColor = (PFNGLCLAMPCOLORPROC)getProcAddress("glClampColor");
	glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)getProcAddress("glBeginConditionalRender");
	glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)getProcAddress("glEndConditionalRender");
	glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)getProcAddress("glVertexAttribIPointer");
	glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)getProcAddress("glGetVertexAttribIiv");
	glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)getProcAddress("glGetVertexAttribIuiv");
	glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)getProcAddress("glVertexAttribI1i");
	glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)getProcAddress("glVertexAttribI2i");
	glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)getProcAddress("glVertexAttribI3i");
	glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)getProcAddress("glVertexAttribI4i");

/*
 = (PFNGLVERTEXATTRIBI1UIPROC)getProcAddress("(GLuint index, GLuint x)");
 = (PFNGLVERTEXATTRIBI2UIPROC)getProcAddress("(GLuint index, GLuint x, GLuint y)");
 = (PFNGLVERTEXATTRIBI3UIPROC)getProcAddress("(GLuint index, GLuint x, GLuint y, GLuint z)");
 = (PFNGLVERTEXATTRIBI4UIPROC)getProcAddress("(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)");
 = (PFNGLVERTEXATTRIBI1IVPROC)getProcAddress("(GLuint index, const GLint *v)");
 = (PFNGLVERTEXATTRIBI2IVPROC)getProcAddress("(GLuint index, const GLint *v)");
 = (PFNGLVERTEXATTRIBI3IVPROC)getProcAddress("(GLuint index, const GLint *v)");
 = (PFNGLVERTEXATTRIBI4IVPROC)getProcAddress("(GLuint index, const GLint *v)");
 = (PFNGLVERTEXATTRIBI1UIVPROC)getProcAddress("(GLuint index, const GLuint *v)");
 = (PFNGLVERTEXATTRIBI2UIVPROC)getProcAddress("(GLuint index, const GLuint *v)");
 = (PFNGLVERTEXATTRIBI3UIVPROC)getProcAddress("(GLuint index, const GLuint *v)");
 = (PFNGLVERTEXATTRIBI4UIVPROC)getProcAddress("(GLuint index, const GLuint *v)");
 = (PFNGLVERTEXATTRIBI4BVPROC)getProcAddress("(GLuint index, const GLbyte *v)");
 = (PFNGLVERTEXATTRIBI4SVPROC)getProcAddress("(GLuint index, const GLshort *v)");
 = (PFNGLVERTEXATTRIBI4UBVPROC)getProcAddress("(GLuint index, const GLubyte *v)");
 = (PFNGLVERTEXATTRIBI4USVPROC)getProcAddress("(GLuint index, const GLushort *v)");
 = (PFNGLGETUNIFORMUIVPROC)getProcAddress("(GLuint program, GLint location, GLuint *params)");
 = (PFNGLBINDFRAGDATALOCATIONPROC)getProcAddress("(GLuint program, GLuint color, const GLchar *name)");
 = (PFNGLGETFRAGDATALOCATIONPROC)getProcAddress("(GLuint program, const GLchar *name)");
 = (PFNGLUNIFORM1UIPROC)getProcAddress("(GLint location, GLuint v0)");
 = (PFNGLUNIFORM2UIPROC)getProcAddress("(GLint location, GLuint v0, GLuint v1)");
 = (PFNGLUNIFORM3UIPROC)getProcAddress("(GLint location, GLuint v0, GLuint v1, GLuint v2)");
 = (PFNGLUNIFORM4UIPROC)getProcAddress("(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)");
 = (PFNGLUNIFORM1UIVPROC)getProcAddress("(GLint location, GLsizei count, const GLuint *value)");
 = (PFNGLUNIFORM2UIVPROC)getProcAddress("(GLint location, GLsizei count, const GLuint *value)");
 = (PFNGLUNIFORM3UIVPROC)getProcAddress("(GLint location, GLsizei count, const GLuint *value)");
 = (PFNGLUNIFORM4UIVPROC)getProcAddress("(GLint location, GLsizei count, const GLuint *value)");

 = (PFNGLTEXPARAMETERIIVPROC)getProcAddress("(GLenum target, GLenum pname, const GLint *params)");
 = (PFNGLTEXPARAMETERIUIVPROC)getProcAddress("(GLenum target, GLenum pname, const GLuint *params)");
 = (PFNGLGETTEXPARAMETERIIVPROC)getProcAddress("(GLenum target, GLenum pname, GLint *params)");
 = (PFNGLGETTEXPARAMETERIUIVPROC)getProcAddress("(GLenum target, GLenum pname, GLuint *params)");
 = (PFNGLCLEARBUFFERIVPROC)getProcAddress("(GLenum buffer, GLint drawbuffer, const GLint *value)");
 = (PFNGLCLEARBUFFERUIVPROC)getProcAddress("(GLenum buffer, GLint drawbuffer, const GLuint *value)");
 = (PFNGLCLEARBUFFERFVPROC)getProcAddress("(GLenum buffer, GLint drawbuffer, const GLfloat *value)");
 = (PFNGLCLEARBUFFERFIPROC)getProcAddress("(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)");
*/
	glGetStringi = (PFNGLGETSTRINGIPROC)getProcAddress("glGetStringi");
	glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)getProcAddress("glIsRenderbuffer");
	glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)getProcAddress("glBindRenderbuffer");
	glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)getProcAddress("glDeleteRenderbuffers");
	glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)getProcAddress("glGenRenderbuffers");
	glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)getProcAddress("glRenderbufferStorage");
	glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)getProcAddress("glGetRenderbufferParameteriv");
	glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)getProcAddress("glIsFramebuffer");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)getProcAddress("glBindFramebuffer");
	glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)getProcAddress("glDeleteFramebuffers");
	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)getProcAddress("glGenFramebuffers");
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)getProcAddress("glCheckFramebufferStatus");
	glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)getProcAddress("glFramebufferTexture1D");
	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)getProcAddress("glFramebufferTexture2D");
	glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)getProcAddress("glFramebufferTexture3D");
	glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)getProcAddress("glFramebufferRenderbuffer");
	glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)getProcAddress	("glGetFramebufferAttachmentParameteriv");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)getProcAddress("glGenerateMipmap");
	glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)getProcAddress("glBlitFramebuffer");
	glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)getProcAddress("glRenderbufferStorageMultisample");
	glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)getProcAddress("glFramebufferTextureLayer");
	glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)getProcAddress("glMapBufferRange");
	glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)getProcAddress("glFlushMappedBufferRange");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)getProcAddress("glBindVertexArray");
	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)getProcAddress("glDeleteVertexArrays");
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)getProcAddress("glGenVertexArrays");
	glIsVertexArray = (PFNGLISVERTEXARRAYPROC)getProcAddress("glIsVertexArray");
}
