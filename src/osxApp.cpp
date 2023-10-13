#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "fi/app/osxApp.h"

#include <mach-o/dyld.h>
#include <unistd.h>

using namespace std;

OSXApp::OSXApp(const int argc, const char *argv[]) :
	Application(argc,argv)
{
}

OSXApp::~OSXApp()
{
}

void OSXApp::createWindow(const char *title, int x, int y, int width, int height, bool fullscreen)
{
	m_xpos = x;
	m_ypos = y;
	m_width = width;
	m_height = height;
	m_bFullscreen = fullscreen;
	
	// change working dir to where the exe is
	char pathbuf[PATH_MAX + 1];
	char pathbufR[PATH_MAX + 1];
	unsigned int bufsize = sizeof(pathbuf);
	_NSGetExecutablePath( pathbuf, &bufsize);
	realpath(pathbuf, pathbufR);

	string path = pathbufR;
	size_t endpos = path.find("MacOS");
	if ( endpos == string::npos ) {
		cout << "Warning: cannot find executable path in package location." << endl;
	} else {
		path.erase(path.begin()+endpos, path.end());
		path += "MacOS";
		chdir( path.c_str() );
		printf("Executable Path: %s\n", path.c_str());
	}

	CWOpenWindow(x, y, width, height, 0, &m_scaleFactor);
	cout << "content scale factor: " << m_scaleFactor << endl;
	m_width = width*m_scaleFactor; // set render surface to be right backing size irrespective of 'DPI'
	m_height = height*m_scaleFactor; // application->event coordinate normalization uses scale factor

	// get display res
	CWGetScreenSize(m_screenWidth, m_screenHeight);

	gfxAPIInit();
}

void OSXApp::destroyWindow()
{
	gfxAPIDeinit();
}

void OSXApp::mainloop()
{
	bool bLeftBtnDown = false;
	bool bRightBtnDown = false;

	while(!m_bQuit) 
	{
		if(CWCheckQuitMessage())
			return;
			
		CWPollDevice();		// handle events
		
		int lb, mb, rb, mx, my;
		int mevent = CWGetMouseEvent(&lb, &mb, &rb, &mx, &my);
		FI::Event e;
		
		switch(mevent) 
		{
			case MOUSEEVENT_LBUTTONDOWN:
				e.setType(FI::EVENT_MOUSE_LEFT_CLICK);
				e.setData((float)mx, (float)my);
				setEvent(e);
				bLeftBtnDown = true;
				break;

			case MOUSEEVENT_LBUTTONUP:
				e.setType(FI::EVENT_MOUSE_LEFT_RELEASE);
				e.setData((float)mx, (float)my);
				setEvent(e);
				bLeftBtnDown = false;
				break;

			case MOUSEEVENT_RBUTTONDOWN:
				e.setType(FI::EVENT_MOUSE_RIGHT_CLICK);
				e.setData((float)mx, (float)my);
				setEvent(e);
				bRightBtnDown = true;
				break;

			case MOUSEEVENT_RBUTTONUP:
				e.setType(FI::EVENT_MOUSE_RIGHT_RELEASE);
				e.setData((float)mx, (float)my);
				setEvent(e);
				bRightBtnDown = false;
				break;

			case MOUSEEVENT_MBUTTONDOWN:
				//e.setType(FI::EVENT_MOUSE_MIDDLE_RELEASE);
				//e.setData((float)mx, (float)my);
				//setEvent(e);
				break;

			case MOUSEEVENT_MOVE:
				if (bLeftBtnDown)
				{
					e.setType(FI::EVENT_MOUSE_LEFT_DRAG);
					e.setData((float)mx, (float)my);
				}
				else if (bRightBtnDown)
				{
					e.setType(FI::EVENT_MOUSE_RIGHT_DRAG);
					e.setData((float)mx, (float)my);
				}
				else
				{
					e.setType(FI::EVENT_MOUSE_MOVE);
					e.setData((float)mx, (float)my);
				}
				setEvent(e);
				break;
			default:
				break;
		}

		if (eKeyCode kc = CWInkey())
		{
			if (CWGetKeyState(kc) == 1)
			{
				e.setType(FI::EVENT_KEY_PRESS);
			}
			else
			{
				e.setType(FI::EVENT_KEY_RELEASE);
			}
			e.setData((unsigned int)kc, 0);
			setEvent(e);
		}

		gfxAPIDraw();		// our draw call
		CWSwapBuffers();
		//CWSleep(17);
	}
}

void OSXApp::warpMouseCursorPosition(unsigned x, unsigned y)
{
	CWWarpMouseCursorPosition(x, y);
}

void OSXApp::swapBuffers()
{
	CWSwapBuffers();
}
