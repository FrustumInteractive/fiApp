/*
* webApp.h
*
* Created on: Sept 10, 2023
*	Author: Roger Dass
*	Copyright Frustum Interactive Inc. - All rights reserved.
*/

#ifndef _WEBAPP_H
#define _WEBAPP_H

#include "app.h"

#include <emscripten.h>
#include <emscripten/html5.h>
#include <SDL2/SDL.h>

#if defined(FI_GFX_WEBGPU)
#include <webgpu/webgpu.h>
#if __has_include(<emscripten/html5_webgpu.h>)
#include <emscripten/html5_webgpu.h>
#else
extern "C" WGPUDevice emscripten_webgpu_get_device(void);
#endif
#endif

#if !defined(FI_GFX_WEBGPU)
#ifdef _GLES3
#include <GLES3/gl3.h>
#endif

#ifdef _GLES2
#include <SDL_opengles2.h>
#endif
#endif

class WebApp : public Application
{
public:
	WebApp(const int argc = 0, const char *argv[] = 0);
	virtual ~WebApp();

	void createWindow(const char *title, int x, int y, int width, int height, bool fullscreen) override;
	void destroyWindow() override;
	void setRelativeMouseMode(bool enabled) override;

	void mainloop() override;

protected:
	void swapBuffers();
	void syncDrawableSize();

	SDL_Window *mWindow = nullptr;
	bool m_leftBtnDown = false;
	bool m_middleBtnDown = false;
	bool m_rightBtnDown = false;
	bool m_relativeMouseMode = false;

#if defined(FI_GFX_WEBGPU)
	WGPUDevice m_wgpuDevice = nullptr;
#endif
};

#endif /*_WEBAPP_H*/
