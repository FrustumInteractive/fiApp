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
#include <SDL2/SDL.h>
#ifdef _GLES3
#include <GLES3/gl3.h>
#endif

#ifdef _GLES2
#include <SDL_opengles2.h>
#endif

class WebApp : public Application
{
public:
	WebApp(const int argc = 0, const char *argv[] = 0);
	virtual ~WebApp();

	void createWindow(const char *title, int x, int y, int width, int height, bool fullscreen) override;
	void destroyWindow() override;

	void mainloop() override;

protected:
	void swapBuffers();

	SDL_Window *mWindow;
};

#endif /*_WEBAPP_H*/
