/*
* mobileApp.h
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include <stdlib.h>
#include <iostream>
#include <string>

#include "application/mobileApp.h"

#ifndef __ANDROID_API__
#include <mach-o/dyld.h>
#endif

using namespace std;

MobileApp::MobileApp(const int argc, const char *argv[]) :
	Application(argc, argv)
{
}

MobileApp::~MobileApp()
{
}

void MobileApp::createWindow(const char *title, int x, int y, int width, int height, bool fullscreen)
{
	m_xpos = x;
	m_ypos = y;
	m_width = width;
	m_height = height;
	m_bFullscreen = fullscreen;
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
}

void MobileApp::destroyWindow()
{
	gfxAPIDeinit();
}

void MobileApp::mainloop()
{
// this probably isnt needed for mobile
	while(!m_bQuit) 
	{
		//if(CWCheckQuitMessage())
			//return;
			
		//CWPollDevice();		// handle events
		/*
		int lb, mb, rb, mx, my;
		int mevent = CWGetMouseEvent(&lb, &mb, &rb, &mx, &my);
		
		switch(mevent) {
		
		case MOUSEEVENT_LBUTTONDOWN:
			onLeftMouseClick(mx,my);
			break;
		case MOUSEEVENT_RBUTTONDOWN:
			onRightMouseClick(mx,my);
			break;
		case MOUSEEVENT_MBUTTONDOWN:
			onMiddleMouseClick(mx,my);
			break;
		default:
			break;
		}
		
		if(eKeyCode kc = CWInkey()){
			onKeyPress(kc);
		}
*/
		gfxAPIDraw();		// our draw call
		//glFlush();		// nothing is  rendered is this isn’t called
		//CWSwapBuffers();
		//CWSleep(17);
	}

}

void MobileApp::swapBuffers()
{
	//CWSwapBuffers();
}
