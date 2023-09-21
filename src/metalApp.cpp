/*
* MetalApp.cpp
*
*  Created on: Aug 23, 2022
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#define NS_PRIVATE_IMPLEMENTATION
//#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "fi/app/metalApp.h"
#include "fi/debug/trace.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>


MetalApp::MetalApp(const int argc, const char *argv[])
#if defined(OSX)
	:OSXApp(argc, argv)
#else
	:MobileApp(argc, argv)
#endif
{
}

MetalApp::~MetalApp()
{
}

void MetalApp::resize(int width, int height)
{
}

void MetalApp::gfxAPIInit()
{
	initScene();
}

void MetalApp::gfxAPIDraw()
{
	drawScene();
}

void MetalApp::gfxAPIDeinit()
{
	deinitScene();
}

