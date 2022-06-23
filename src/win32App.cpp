/*
* win32App.cpp
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include <iostream>

#include "Application/console.h"
#include "Application/Win32App.h"

using namespace std;

Win32App::Win32App(const int argc, const char *argv[]) :
	Application(argc, argv),
	m_bitsPerPixel(24),
	m_bKeys()
{
	m_bFullscreen = false;
	m_bActive = true;
}


void Win32App::mainloop()
{
	while(!m_bQuit)
	{
		// Is There A Message Waiting?
		if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_msg.message == WM_QUIT) {
				m_bQuit = true;
			}
			else {
				TranslateMessage(&m_msg);
				DispatchMessage(&m_msg);
			}
		}
		else
		{
			// Draw The Scene.
			if (m_bKeys[VK_ESCAPE])	onKeyPress(KEY_ESC); 
			else if(m_bKeys['Q']) onKeyPress(KEY_Q); //VK_A..Z maps to ASCII
			else if (m_bKeys[VK_F1]) onKeyPress(KEY_F1);
			/*	
			{	
				destroyWindow();		// Kill Our Current Window
				m_bFullscreen=!m_bFullscreen;	// Toggle Fullscreen / Windowed Mode
				
				int w = m_bFullscreen ? m_fullscreenRECT.right : m_windowRECT.right;
				int h = m_bFullscreen ? m_fullscreenRECT.bottom : m_windowRECT.bottom;

				createWindow(m_appName, m_xpos, m_ypos, w, h, m_bFullscreen);
			}
			*/
			else {
				gfxAPIDraw();
				SwapBuffers(m_hDC);
				Sleep(16.667);
			}
		}
	}
}


void Win32App::createWindow(const char *title, int x, int y, int width, int height, bool fullscreen)
{
#ifdef _DEBUG
	//RedirectIOToConsole();
#endif

	m_appName = m_title = title;

	m_xpos = x;
	m_ypos = y;
	m_width = width;
	m_height = height;
	m_bFullscreen = fullscreen;

	if(m_bFullscreen)
	{
		m_width = GetSystemMetrics(SM_CXSCREEN);
		m_height = GetSystemMetrics(SM_CYSCREEN);
	}

	// First some standard Win32 window creating
	WNDCLASS	wc;
	DWORD		dwExStyle;	
	DWORD		dwStyle;	
	RECT		WindowRect;
	WindowRect.left=(long)m_xpos;
	WindowRect.right=(long)m_width;
	WindowRect.top=(long)m_ypos;
	WindowRect.bottom=(long)m_height;

	m_hInstance		= GetModuleHandle(NULL);
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	= (WNDPROC) staticWndProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= m_hInstance;
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);	
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;			
	wc.lpszMenuName		= NULL;		
	wc.lpszClassName	= m_appName;	

	// Register the window class
	if (!RegisterClass(&wc))		
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return ;//FALSE;
	}

	/*
	if (m_bFullscreen)												// Attempt Fullscreen Mode?
	{
		int bits=4;
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= m_width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= m_height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", m_appName, MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				m_bFullscreen=false;	// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return ;//FALSE;									// Return FALSE
			}
		}
	}
	*/

	if (m_bFullscreen)
	{
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;	
		ShowCursor(FALSE);	
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	
		dwStyle=WS_OVERLAPPEDWINDOW;	
	}

    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

    // Create The Window
    if (!(m_hWnd = CreateWindowEx(	
			dwExStyle,	
			m_appName,	
			m_title,		
			dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			CW_USEDEFAULT, 
			CW_USEDEFAULT,				
			WindowRect.right-WindowRect.left,
			WindowRect.bottom-WindowRect.top,
			NULL,			
			NULL,		
			m_hInstance,		
			this)))	
    {
		destroyWindow();		// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return ;//FALSE;
	}

	if (!(m_hDC = GetDC(m_hWnd)))	// Did We Get A Device Context?	
	{
		destroyWindow();		// Reset The Display
		MessageBox(NULL, "Can't Create A Device Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return ;//FALSE;		// Return FALSE
	}

	ShowWindow( m_hWnd, SW_SHOW );	// Show The Window
	SetForegroundWindow( m_hWnd );	// Slightly Higher Priority
	SetFocus( m_hWnd );				// Sets Focus To The Window

	resize( m_width, m_height );		// Set Up Our Perspective D3D Screen

	gfxAPIInit();	//initialize graphics subsystem
}

void Win32App::resize(int width, int height)
{
	if (height==0)				// Prevent A Divide By Zero By Making Height Equal One
		height=1;

	m_width = width;
	m_height = height;
}

LRESULT CALLBACK Win32App::staticWndProc(
		HWND hWnd, 
		UINT uMsg, 
		WPARAM wParam, 
		LPARAM lParam)
{
	Win32App *pParent;   // Get pointer to window
	if(uMsg == WM_CREATE)   
	{      
		pParent = (Win32App*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd,GWL_USERDATA,(LONG_PTR)pParent);   
	}  
 	else   
	{      
		pParent = (Win32App*)GetWindowLongPtr(hWnd,GWL_USERDATA);
		if(!pParent) 
			return DefWindowProc(hWnd,uMsg,wParam,lParam);   
	}   

	pParent->m_hWnd = hWnd;   
	return pParent->WndProc(uMsg,wParam,lParam);
}



LRESULT Win32App::WndProc(
		UINT	uMsg,	// Message For This Window
		WPARAM	wParam,	// Additional Message Information
		LPARAM	lParam)	// Additional Message Information
{
	switch (uMsg)			// Check For Windows Messages
	{
		case WM_ACTIVATE:	// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))	// Check Minimization State
			{
				m_bActive = true;	// Program Is Active
			}
			else
			{
				m_bActive = false;	// Program Is No Longer Active
			}

			return 0;	// Return To The Message Loop
		}

		case WM_SYSCOMMAND:	// Intercept System Commands
		{
			switch (wParam)	// Check System Calls
			{
				// Screensaver Trying To Start?
				case SC_SCREENSAVE:	
				// Monitor Trying To Enter Powersave?
				case SC_MONITORPOWER:
				return 0;	// Prevent From Happening
			}
			break;			// Exit
		}

		case WM_CLOSE:	// Did We Receive A Close Message?
		{
			PostQuitMessage(0);// Send A Quit Message
			return 0;		// Jump Back
		}

		case WM_KEYDOWN:		// Is A Key Being Held Down?
		{
			m_bKeys[wParam] = true;	// If So, Mark It As TRUE
			return 0;		// Jump Back
		}

		case WM_KEYUP:		// Has A Key Been Released?
		{
			m_bKeys[wParam] = false;// If So, Mark It As FALSE
			return 0;	// Jump Back
		}

		case WM_SIZE:		// Resize The Direct3D Window
		{
 			// LoWord=Width, HiWord=Height
			resize(LOWORD(lParam), HIWORD(lParam));
			return 0;			// Jump Back
		}
		
		case WM_LBUTTONDOWN:
			onLeftMouseClick(LOWORD(lParam), HIWORD(lParam));
			return 0;
			
		case WM_LBUTTONUP:
			onLeftMouseRelease(LOWORD(lParam), HIWORD(lParam));
			return 0;
			
		case WM_MBUTTONDOWN:
			onMiddleMouseClick(LOWORD(lParam), HIWORD(lParam));
			return 0;
			
		case WM_MBUTTONUP:
			onMiddleMouseRelease(LOWORD(lParam), HIWORD(lParam));
			return 0;
			
		case WM_RBUTTONDOWN:
			onRightMouseClick(LOWORD(lParam), HIWORD(lParam));
			return 0;
			
		case WM_RBUTTONUP:
			onRightMouseRelease(LOWORD(lParam), HIWORD(lParam));
			return 0;
			
		case WM_MOUSEMOVE:
			switch(wParam)
			{
				case MK_LBUTTON:
					onLeftMouseDrag(LOWORD(lParam), HIWORD(lParam));
					return 0;
					
				case MK_RBUTTON:
					onRightMouseDrag(LOWORD(lParam), HIWORD(lParam));
					return 0;
					
				case MK_MBUTTON:
					onMiddleMouseDrag(LOWORD(lParam), HIWORD(lParam));
					return 0;
			}
			
			onMouseMove(LOWORD(lParam), HIWORD(lParam));
			return 0;
	}

	return DefWindowProc( m_hWnd, uMsg, wParam, lParam);	// Pass All Unhandled Messages To DefWindowProc
}

void Win32App::destroyWindow()
{
	gfxAPIDeinit();	//cleanup graphics API stuff

	if (m_bFullscreen)									// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// switch Back To Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (m_hDC && !ReleaseDC(m_hWnd, m_hDC))				// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_hDC = NULL;		
	}

	if (m_hWnd && !DestroyWindow(m_hWnd))				// Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		m_hWnd=NULL;	
	}

	if (!UnregisterClass( m_appName, m_hInstance ))	// Able To Unregister Class?
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		m_hInstance=NULL;								// Set hInstance To NULL
	}
}

