/*
* win32App.h
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#ifndef _WIN32APP_H
#define _WIN32APP_H

#include <Windows.h>
#include "app.h"

class Win32App : public Application
{
public:
	Win32App(const int argc = 0, const char *argv[] = 0);
	virtual ~Win32App(){}

	virtual void createWindow(const char *title, int x, int y, int width, int height, bool fullscreen);
	virtual void destroyWindow();
	virtual void mainloop();

	virtual void resize(int width, int height);

	// keyboard
	virtual void onKeyPress(eKeyCode kc){}
	virtual void onKeyRelease(eKeyCode kc){}

	// mouse
	virtual void onMouseMove(int x, int y){}
	virtual void onLeftMouseClick(int x, int y){}
	virtual void onLeftMouseRelease(int x, int y){}
	virtual void onLeftMouseDrag(int x, int y ){}
	virtual void onMiddleMouseClick(int x, int y){}
	virtual void onMiddleMouseRelease(int x, int y){}
	virtual void onMiddleMouseDrag(int x, int y){}
	virtual void onRightMouseClick(int x, int y){}
	virtual void onRightMouseRelease(int x, int y){}
	virtual void onRightMouseDrag(int x, int y){}

protected:

	HDC			m_hDC;        // Private GDI Device Context
	HGLRC		m_hRC;        // Permanent Rendering Context
	HWND		m_hWnd;       // Holds Our Window Handle
	HINSTANCE	m_hInstance;  // Holds The Instance Of The Application
	LPCSTR		m_appName;    // hold name of application

	int m_bitsPerPixel;

	RECT
		m_windowRECT,
		m_fullscreenRECT;

	bool m_bKeys[256];     // Array Used For The Keyboard Routine

private:

	MSG	m_msg;

	static LRESULT CALLBACK staticWndProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam );

	LRESULT WndProc(
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam );

};



#endif /*_WIN32APP_H*/
