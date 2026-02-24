/*
* cocoaWrapper.cpp
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#if FI_GFX_METAL
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include "fi/app/appDefines.h"

extern "C" void CWOpenWindowC(int x0,int y0,int wid,int hei,int useDoubleBuffer, float *scaleFactor);
extern "C" void CWGetWindowSizeC(int *wid,int *hei);
extern "C" void CWGetScreenSizeC(int *w, int *h);
extern "C" void CWPollDeviceC(void);
extern "C" void CWSleepC(int ms);
extern "C" int CWPassedTimeC(void);
extern "C" void CWMouseC(int *lb,int *mb,int *rb,int *mx,int *my);
extern "C" int CWGetMouseEventC(int *lb,int *mb,int *rb,int *mx,int *my);
extern "C" void CWSwapBufferC(void);
extern "C" eKeyCode CWInkeyC(void);
extern "C" char CWInkeyCharC(void);
extern "C" int CWKeyStateC(eKeyCode kc);
extern "C" void CWChangeToProgramDirC(void);
extern "C" int CWCheckExposureC(void);
extern "C" int CWCheckQuitMessageC(void);
extern "C" void CWWarpMouseCursorPositionC(unsigned x, unsigned y);

#if FI_GFX_METAL
	extern "C" void* CWGetMetalLayerC(void);
	extern "C" void* CWGetNativeViewC(void);

	void* CWGetMetalLayer() { return CWGetMetalLayerC(); }
	void* CWGetNativeView() { return CWGetNativeViewC(); }
#endif



void CWOpenWindow(int x0, int y0, int width, int height, int useDoubleBuffer, float *scaleFactor)
{
	CWOpenWindowC(x0,y0,width,height,useDoubleBuffer, scaleFactor);
}

void CWGetWindowSize(int &width,int &height)
{
	CWGetWindowSizeC(&width, &height);
}

void CWGetScreenSize(int &w, int &h)
{
	CWGetScreenSizeC(&w, &h);
}

void CWPollDevice(void)
{
	CWPollDeviceC();
}

void CWSleep(int ms)
{
	CWSleepC(ms);
}

int CWPassedTime(void)
{
	return CWPassedTimeC();
}

void CWGetMouseState(int *lb, int *mb, int *rb, int *mx, int *my)
{
	CWMouseC(lb,mb,rb,mx,my);
}

int CWGetMouseEvent(int *lb,int *mb,int *rb,int *mx,int *my)
{
	return CWGetMouseEventC(lb,mb,rb,mx,my);
}

void CWSwapBuffers(void)
{
	CWSwapBufferC();
}

eKeyCode CWInkey(void)
{
	return CWInkeyC();
}

char CWInkeyChar(void)
{
	return CWInkeyCharC();
}

int CWGetKeyState(eKeyCode kc)
{
	return CWKeyStateC(kc);
}

int CWCheckWindowExposure(void)
{
	return CWCheckExposureC();
}

int CWCheckQuitMessage(void)
{
	return CWCheckQuitMessageC();
}

void CWChangeToProgramDir(void)
{
	CWChangeToProgramDirC();
}

void CWWarpMouseCursorPosition(unsigned x, unsigned y)
{
	CWWarpMouseCursorPositionC(x, y);
}
