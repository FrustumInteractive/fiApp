/*
* OGLApp.cpp
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include "fi/app/oglApp.h"
#include "debug/trace.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

std::string boolStr(bool b)
{
	return b ? "yes" : "no";
}

void printList(const std::string &str)
{
	char* s = strtok((char*)str.c_str(), " \n\t");
	while(s)
	{
		FI::LOG(s);
		s = strtok(NULL, " \n\t");
	}
}

static bool isExtensionSupported(const char *extList, const char *extension)
{
	const char *start, *where, *terminator;

	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');
	if (where || *extension == '\0') return false;

	for (start=extList;;)
	{
		where = strstr(start, extension);
		if (!where)	break;
		terminator = where + strlen(extension);
		if ( where == start || *(where - 1) == ' ' )
			if ( *terminator == ' ' || *terminator == '\0' ) return true;
		start = terminator;
	}
	return false;
}

#if defined(LINUX)
//#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
//#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static bool ctxErrorOccurred = false;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
	ctxErrorOccurred = true;
	return 0;
}
#endif

// statics
int OGLApp::m_majorVersion = 0;
int OGLApp::m_minorVersion = 0;
bool OGLApp::m_es2Compatibility = false;
bool OGLApp::m_es3Compatibility = false;
void *OGLApp::m_glContextObj = nullptr;
void *OGLApp::m_glPixelFormatObj = nullptr;

OGLApp::OGLApp(const int argc, const char *argv[])
#if defined(WIN32)
	:Win32App(argc, argv)
#elif defined(LINUX)
	:X11App(argc, argv)
#elif defined(OSX)
	:OSXApp(argc, argv)
#else
	:MobileApp(argc, argv)
#endif
	,m_pixelFormat(0)
{
}

OGLApp::~OGLApp()
{
}

void* OGLApp::platformGLContextObj()
{
	return m_glContextObj;
}

void* OGLApp::platformGLPixelFormatObj()
{
	return m_glPixelFormatObj;
}

void OGLApp::resize(int width, int height)
{

}

void OGLApp::gfxAPIInit()
{
#ifdef WIN32
	FI::LOG("OpenGL App: Initializing OpenGL Subsystem...");
	static	PIXELFORMATDESCRIPTOR pfd=		// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),		// Size Of This Pixel Format Descriptor
		1,							// Version Number
		PFD_DRAW_TO_WINDOW |		// Format Must Support Window
		PFD_SUPPORT_OPENGL |		// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,			// Must Support Double Buffering
		PFD_TYPE_RGBA,				// Request An RGBA Format
		(BYTE)m_bitsPerPixel,		// Select Our Color Depth
		0, 0, 0, 0, 0, 0,			// Color Bits Ignored
		0,					// No Alpha Buffer
		0,					// Shift Bit Ignored
		0,					// No Accumulation Buffer
		0, 0, 0, 0,			// Accumulation Bits Ignored
		32,					// 16Bit Z-Buffer (Depth Buffer)  
		0,					// No Stencil Buffer
		0,					// No Auxiliary Buffer
		PFD_MAIN_PLANE,		// Main Drawing Layer
		0,					// Reserved
		0, 0, 0				// Layer Masks Ignored
	};

	if (!(m_pixelFormat=ChoosePixelFormat(m_hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return;								// Return FALSE
	}

	if(!SetPixelFormat(m_hDC, m_pixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return;								// Return FALSE
	}

	if (!(m_hRC=wglCreateContext(m_hDC)))				// Are We Able To Get A Rendering Context?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR", MB_OK|MB_ICONEXCLAMATION);
		return;								// Return FALSE
	}

	if(!wglMakeCurrent(m_hDC, m_hRC))					// Try To Activate The Rendering Context
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR", MB_OK|MB_ICONEXCLAMATION);
		return;								// Return FALSE
	}
#endif /*WIN32*/


#ifdef LINUX

	// Get the default screen's GLX extension list
  	const char *glxExts = glXQueryExtensionsString(m_display, DefaultScreen(m_display));

  	// NOTE: It is not necessary to create or make current to a context before
  	// calling glXGetProcAddressARB
  	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

	// Install an X error handler so the application won't exit if GL 3.0
  	// context allocation fails.
  	//
  	// Note this error handler is global.  All display connections in all threads
  	// of a process use the same error handler, so be sure to guard against other
  	// threads issuing X commands while this code is running.
  	ctxErrorOccurred = false;
  	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, use GLX 1.3 context creation method.
	if(!isExtensionSupported( glxExts, "GLX_ARB_create_context" ) || !glXCreateContextAttribsARB )
	{
		FI::LOG("glXCreateContextAttribsARB() not found... using old-style GLX context");
		if(m_glxMinor > 2)
			m_glContext = glXCreateNewContext( m_display, m_bestFbc, GLX_RGBA_TYPE, 0, True );
		else
			m_glContext = glXCreateContext(m_display, m_visualInfo, NULL, GL_TRUE);
	}
	else // If it does, try to get a GL 3.2 context!
	{
		int context_attribs[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 2,
			GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			GLX_CONTEXT_PROFILE_MASK_ARB , GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
			None
		};

		printf( "Creating context" );
		m_glContext = glXCreateContextAttribsARB(m_display, m_bestFbc, 0, True, context_attribs );

		// Sync to ensure any errors generated are processed.
		XSync( m_display, False );
		if ( !ctxErrorOccurred && m_glContext )
			FI::LOG( "Created GL 3.2 context" );
		else
		{
			// Couldn't create GL 3.2 context.  Fall back to old-style 2.x context.
			// When a context version below 3.0 is requested, implementations will
			// return the newest context version compatible with OpenGL versions less
			// than version 3.0.
			// GLX_CONTEXT_MAJOR_VERSION_ARB = 1
			context_attribs[1] = 1;
			// GLX_CONTEXT_MINOR_VERSION_ARB = 0
			context_attribs[3] = 0;

			ctxErrorOccurred = false;

			FI::LOG("Failed to create GL 3.2 context... using old-style GLX context");
			m_glContext = glXCreateContextAttribsARB(m_display, m_bestFbc, 0, True, context_attribs );
		}
	}

	// Sync to ensure any errors generated are processed.
	XSync( m_display, False );

	// Restore the original error handler
	XSetErrorHandler( oldHandler );

	if ( ctxErrorOccurred || !m_glContext )
	{
		FI::LOG( "failed to create an OpenGL context" );
		exit(1);
	}

	// Verifying that context is a direct context
	if (!glXIsDirect(m_display, m_glContext))
	{
		FI::LOG( "indirect GLX rendering context obtained" );
	}
	else
	{
		FI::LOG( "direct GLX rendering context obtained" );
	}

	FI::LOG( "making context current" );
	glXMakeCurrent( m_display, m_window, m_glContext );

#endif /*LINUX*/

#if !defined(IOS) && !defined(MISC) /*&& !defined(OSX)*/

	// Load linux GL 2.1 functions
	FI::LOG("loading OpenGL procedures...");

	open_libgl();
	initGL1XFuncs();
	initGL20Funcs();

	glGetIntegerv(GL_MAJOR_VERSION, &m_majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &m_minorVersion);

	#if defined(_ANDROID)
		// android hack to force to GLES 2
		if(m_majorVersion > 2)
			m_majorVersion = 2;
	#endif

	if(!m_majorVersion)
	{
		// try to get version in a more hacky way
		std::string verStr((const char*)glGetString(GL_VERSION));
		std::string ver(verStr.substr(0,3));
		m_majorVersion = std::stoi(ver.substr(0,1));
		m_minorVersion = std::stoi(ver.substr(2,1));
	}
	FI::LOG("------------------ GL INFO -------------------");
	FI::LOG(" OpenGL:", glGetString(GL_VERSION), "[Major:", m_majorVersion, "Minor:", m_minorVersion, "]");
	FI::LOG(" GLSL:", glGetString(GL_SHADING_LANGUAGE_VERSION));
	FI::LOG(" Vendor:", glGetString(GL_VENDOR));
	FI::LOG(" Renderer:", glGetString(GL_RENDERER));

	bool vaoSupport = false;
	bool vboSupport = false;
	bool fboSupport = false;

	if(m_majorVersion > 2)
	{
		initGL21Funcs();

		if (m_majorVersion == 3)
		{
			switch (m_minorVersion)
			{
				case 3:
				case 2: initGL32Funcs();
				case 1: initGL31Funcs();
				case 0: initGL30Funcs();
			}
		}
		else
		{
			initGL30Funcs();
			initGL31Funcs();
			initGL32Funcs();
		}

		int numExt = 0, count = 0;

		glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
		std::vector<std::string> exts;
		exts.resize(numExt);

		// load new glGetstringi proc
		glGetStringi = (PFNGLGETSTRINGIPROC)getProcAddress("glGetStringi");

		FI::LOG(" ", numExt, "GL extensions found.");

		for(auto i : exts)
		{
			i = std::string((const char*)glGetStringi(GL_EXTENSIONS, count));

			m_es3Compatibility = i == "GL_ARB_ES3_compatibility";
			m_es2Compatibility = i == "GL_ARB_ES2_compatibility";
			vboSupport = i == "GL_ARB_vertex_buffer_object";
			vaoSupport = i == "GL_ARB_vertex_array_object";
			fboSupport = i == "GL_ARB_framebuffer_object";
		}
	}
	else
	{
		// we're using a legacy context most likely
		initGL21Funcs();

		const GLubyte* exts = glGetString(GL_EXTENSIONS);
		if(exts)
		{
			// go through all extensions and look for important onces
			size_t numExts = 0;
			char* s = strtok((char*)exts, " \n\t");
			while(s)
			{
				//FI::LOG(s);
				if(strstr((const char*)s, "GL_ARB_vertex_array_object")) vaoSupport = true;
				if(strstr((const char*)s, "GL_ARB_vertex_buffer_object")) vboSupport = true;
				if(strstr((const char*)s, "GL_ARB_framebuffer_object")) fboSupport = true;
				if(strstr((const char*)s, "GL_ARB_ES2_compatibility")) m_es2Compatibility = true;
				if(strstr((const char*)s, "GL_ARB_ES3_compatibility")) m_es3Compatibility = true;

				s = strtok(NULL, " \n\t");
				numExts++;
			}
			FI::LOG(" found", numExts, "GL extensions.");

			// load procedures for specific extensions
			// to get openGL ES 2  base functionality
			if(fboSupport)
			{
				glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)getProcAddress("glIsRenderbuffer");
				glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)getProcAddress("glDeleteRenderbuffers");
				glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)getProcAddress("glGenRenderbuffers");
				glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)getProcAddress("glBindRenderbuffer");

				glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)getProcAddress("glRenderbufferStorage");

				glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)getProcAddress("glRenderbufferStorageMultisample");

				glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)getProcAddress("glGetRenderbufferParameteriv");

				glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)getProcAddress("glIsFramebuffer");
				glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)getProcAddress("glBindFramebuffer");
				glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)getProcAddress("glDeleteFramebuffers");
				glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)getProcAddress("glGenFramebuffers");

				glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)getProcAddress("glCheckFramebufferStatus");

				glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)getProcAddress("glFramebufferTexture1D");
				glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)getProcAddress("glFramebufferTexture2D");
				glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)getProcAddress("glFramebufferTexture3D");
				glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)getProcAddress("glFramebufferTextureLayer");

				glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)getProcAddress("glFramebufferRenderbuffer");

				glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) getProcAddress("glGetFramebufferAttachmentParameteriv");

				glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)getProcAddress("glBlitFramebuffer");

				glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)getProcAddress("glGenerateMipmap");
			}
		}
		else
		{
			FI::LOG(" failed to query extensions via glGetString(GL_EXTENSIONS)");
		}
	}

	FI::LOG("--- Required OpenGL Extension Support List ---");
	FI::LOG(" OpenGL ES 2:", boolStr(m_es2Compatibility));
	FI::LOG(" OpenGL ES 3:", boolStr(m_es3Compatibility));
	FI::LOG(" Vertex Buffer Object:", boolStr(vboSupport));
	FI::LOG(" Vertex Array Object:", boolStr(vaoSupport));
	FI::LOG(" Frame Buffer Object:", boolStr(fboSupport));
	FI::LOG("----------------------------------------------");

#else
	m_majorVersion = 2;
	m_minorVersion = 0;
	m_es2Compatibility = true;
#endif /* IOS && _ANDROID*/
	
#if defined(OSX) || defined(IOS) || defined(MISC)
	m_glContextObj = _glContext;
	m_glPixelFormatObj = _glPixelFormat;
	FI::LOG("OpenGL Context ptr:", m_glContextObj);
	FI::LOG("OpenGL Platform Pixel Format ptr:", m_glPixelFormatObj);
#endif

	initScene();
}

void OGLApp::gfxAPIDraw()
{
	drawScene();
	glFlush();
	m_bRenderRequested = false;
}

void OGLApp::gfxAPIDeinit()
{
	deinitScene();

#ifdef WIN32
	FI::LOG( "OpenGL App: shutting Down OpenGL...");
	if (m_hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBoxA(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(m_hRC))						// Are We Able To Delete The RC?
		{
			MessageBoxA(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		m_hRC=NULL;										// Set RC To NULL
	}
#endif /*WIN32*/
#ifdef LINUX
	glXMakeCurrent(m_display, None, NULL);
	glXDestroyContext(m_display, m_glContext);
#endif /*LINUX*/

#if !defined(IOS) && !defined(ANDROID) && !defined(MISC) /*&& !defined(OSX) */
	close_libgl();
#endif
}

int OGLApp::majorVersion(){return m_majorVersion;}

int OGLApp::minorVersion(){	return m_minorVersion;}

bool OGLApp::es2Compatibility() { return m_es2Compatibility;}
bool OGLApp::es3Compatibility() { return m_es3Compatibility;}
