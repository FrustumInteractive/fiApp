/*
* MetalApp.cpp
*
*  Created on: Aug 23, 2022
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include "fi/app/metalApp.h"
#include "debug/trace.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>


MetalApp::MetalApp(const int argc, const char *argv[])
#if defined(OSX)
	:OSXApp(argc, argv)
#else
	:MobileApp(argc, argv)
#endif
	,m_pixelFormat(0)
{
}

MetalApp::~MetalApp()
{
}

void* MetalApp::platformGLContextObj()
{
	return m_glContextObj;
}

void* MetalApp::platformGLPixelFormatObj()
{
	return m_glPixelFormatObj;
}

void MetalApp::resize(int width, int height)
{

}

void MetalApp::gfxAPIInit()
{
	// init metal specific stuf here 

	initScene();
}

void MetalApp::gfxAPIDraw()
{
	drawScene();

	m_bRenderRequested = false;
}

void MetalApp::gfxAPIDeinit()
{
	deinitScene();

}

int MetalApp::majorVersion(){return m_majorVersion;}

int MetalApp::minorVersion(){	return m_minorVersion;}
