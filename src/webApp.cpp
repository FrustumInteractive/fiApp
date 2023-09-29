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

	SDL_CreateWindowAndRenderer(m_width, m_height, 0, &mWindow, nullptr);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

/*	
	// change working dir to where the exe is
	char pathbuf[PATH_MAX + 1];
	char pathbufR[PATH_MAX + 1];
	unsigned int bufsize = sizeof(pathbuf);
	_NSGetExecutablePath( pathbuf, &bufsize);
	realpath(pathbuf, pathbufR);
	
	string path = pathbufR;
	size_t endpos = path.find("MacOS");
	if( endpos == string::npos ) {
		cout << "Warning: cannot find executable path in package location." << endl;
	}
	else {
		path.erase(path.begin()+endpos, path.end());
		path += "MacOS";
		chdir( path.c_str() );
		printf("Executable Path: %s\n", path.c_str());
	}
	
	CWOpenWindow(x, y, width, height, 0);
*/
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
