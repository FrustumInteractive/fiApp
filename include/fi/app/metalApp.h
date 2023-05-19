/*
* metalApp.h
*
*  Created on: Aug 22, 2022
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#ifndef _METALAPP_H
#define _METALAPP_H

#if defined(OSX)
	#include "osxApp.h"
	#include <Metal/Metal.hpp>
	class MetalApp : public OSXApp
#else
	#ifdef __APPLE__
		#include "TargetConditionals.h"
		#if TARGET_IPHONE_SIMULATOR
			#include <Metal/Metal.hpp>
			#include "mobileApp.h"
			class MetalApp : public MobileApp
		#elif TARGET_OS_IPHONE
			#include <Metal/Metal.hpp>
			#include "mobileApp.h"
			class MetalApp : public MobileApp
		#elif TARGET_OS_MAC
			// Other kinds of Mac OS
			//#include <Metal/Metal.hpp>
			#include "mobileApp.h"
			class MetalApp : public MobileApp
		#else
			#error "Unknown Apple platform"
		#endif
	#endif
#endif
{
public:
	MetalApp(const int argc = 0, const char *argv[] = 0);
	virtual ~MetalApp();

	virtual void initScene() {};
	virtual void deinitScene() {};
	virtual void drawScene() {};

	void resize(int width, int height);
	
	void gfxAPIInit();
	void gfxAPIDraw();
	void gfxAPIDeinit();

	static int majorVersion();
	static int minorVersion();

	static void* platformGLContextObj();
	static void* platformGLPixelFormatObj();

protected:

	int m_pixelFormat;

	static int m_majorVersion;
	static int m_minorVersion;
	static void* m_glContextObj;
	static void* m_glPixelFormatObj;
};

#endif /*_OGLAPP_H*/
