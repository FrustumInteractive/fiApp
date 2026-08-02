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
#include <windowsx.h>
#include "app.h"

class Win32App : public Application
{
public:
	Win32App(const int argc = 0, const char *argv[] = 0);
	virtual ~Win32App(){}

	void createWindow(const char *title, int x, int y, int width, int height, bool fullscreen) override;
	void destroyWindow() override;
	void mainloop() override;

	void resize(int width, int height) override;

	void warpMouseCursorPosition(unsigned int x, unsigned int y) override;
	void warpMouseCursorPositionInWindow(float x, float y) override;
	void setRelativeMouseMode(bool enabled) override;

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
	bool m_hasLastMousePosition = false;
	float m_lastMouseX = 0.0f;
	float m_lastMouseY = 0.0f;

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
