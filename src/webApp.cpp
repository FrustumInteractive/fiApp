/*
* webApp.h
*
*  Created on: Sept 10, 2023
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include <stdlib.h>
#include <iostream>
#include <string>

#include "fi/app/webApp.h"

using namespace std;

void *_glContext = 0;
void *_glPixelFormat = 0;

std::function<void()> loop;
void main_loop()
{
	loop();
}

WebApp::WebApp(const int argc, const char *argv[]) :
	Application(argc, argv)
{

}

WebApp::~WebApp()
{
}

void WebApp::createWindow(const char *title, int x, int y, int width, int height, bool fullscreen)
{
	m_xpos = x;
	m_ypos = y;
	m_width = width;
	m_height = height;
	m_bFullscreen = fullscreen;

	SDL_Init(SDL_INIT_VIDEO);

#ifdef _GLES3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_CreateWindowAndRenderer(m_width, m_height,
		SDL_WINDOW_ALLOW_HIGHDPI,
		&mWindow, nullptr);

	SDL_GL_CreateContext(mWindow);

	const GLubyte* glVersion = glGetString(GL_VERSION);
	printf("GLVersion = %s\n", glVersion);
	
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	printf("vendor = %s\nrenderer = %s\n", vendor, renderer);

	SDL_GL_GetDrawableSize(mWindow, &m_width, &m_height);

	gfxAPIInit();

	loop = [&]()
	{
		mainloop();
	};

	emscripten_set_main_loop(main_loop, 0, true);
}

void WebApp::destroyWindow()
{
	gfxAPIDeinit();

	//TODO: cleanup SDL stuff
}

void WebApp::mainloop()
{
	gfxAPIDraw(); // our draw call
	swapBuffers();
}

void WebApp::swapBuffers()
{
	SDL_GL_SwapWindow(mWindow);
}
