/*
*	OGLApp.h
*
*	Created on: Aug 10, 2015
*		Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#ifndef _OGLAPP_H
#define _OGLAPP_H

#if defined(WIN32)
	#include "win32App.h"
	class OGLApp : public Win32App
#elif defined(LINUX)
	#include "x11App.h"
	#include <fi/gl/glcorearb.h>
	#include <fi/gl/glProcs.h>
	class OGLApp : public X11App
#elif defined(OSX)
	#include "osxApp.h"
	#include <fi/gl/glProcs.h>
	class OGLApp : public OSXApp
#elif defined(__ANDROID__)
	#include <fi/gl/glProcs.h>
	#include "mobileApp.h"
	class OGLApp : public MobileApp
#else
	#ifdef __APPLE__
		#include "TargetConditionals.h"
		#if TARGET_IPHONE_SIMULATOR
			#include <OpenGLES/ES2/gl.h>
			#include <OpenGLES/ES2/glext.h>
			#include "mobileApp.h"
			class OGLApp : public MobileApp
		#elif TARGET_OS_IPHONE
			#include <OpenGLES/ES2/gl.h>
			#include <OpenGLES/ES2/glext.h>
			#include "mobileApp.h"
			class OGLApp : public MobileApp
		#elif TARGET_OS_MAC
			// Other kinds of Mac OS
			#include "mobileApp.h"
			class OGLApp : public MobileApp
		#else
			#error "Unknown Apple platform"
		#endif
	#endif
#endif
{
public:
	OGLApp(const int argc = 0, const char *argv[] = 0);
	virtual ~OGLApp();

	virtual void initScene() {};
	virtual void deinitScene() {};
	virtual void drawScene() {};

	void resize(int width, int height);

	void gfxAPIInit();
	void gfxAPIDraw();
	void gfxAPIDeinit();

	static int majorVersion();
	static int minorVersion();
	static bool es2Compatibility();
	static bool es3Compatibility();

	static void* platformGLContextObj();
	static void* platformGLPixelFormatObj();

protected:

#if defined(LINUX)
	GLXContext	m_glContext;
#elif defined(WIN32)
#elif defined(OSX)
#else
	std::function<void()> m_makeCurrentCallback;
#endif

	int m_pixelFormat;

	static int m_majorVersion;
	static int m_minorVersion;
	static bool m_es2Compatibility;
	static bool m_es3Compatibility;
	static void* m_glContextObj;
	static void* m_glPixelFormatObj;
};

#endif /*_OGLAPP_H*/
