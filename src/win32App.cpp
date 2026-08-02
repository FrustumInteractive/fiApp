/*
* win32App.cpp
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include <iostream>

#include "fi/app/console.h"
#include "fi/app/win32App.h"

using namespace std;

namespace
{
float enableHighDpiRendering()
{
	// Resolve newer DPI APIs dynamically so the same binary remains usable on
	// older Windows versions covered by the Win7 compatibility baseline.
	HMODULE user32 = GetModuleHandleA("user32.dll");
	if (user32)
	{
		using SetProcessDpiAwarenessContextFn = BOOL (WINAPI *)(HANDLE);
		auto setDpiContext = reinterpret_cast<SetProcessDpiAwarenessContextFn>(
			GetProcAddress(user32, "SetProcessDpiAwarenessContext"));
		if (setDpiContext)
		{
			// DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
			setDpiContext(reinterpret_cast<HANDLE>(-4));
		}
		else
		{
			using SetProcessDPIAwareFn = BOOL (WINAPI *)();
			auto setDpiAware = reinterpret_cast<SetProcessDPIAwareFn>(
				GetProcAddress(user32, "SetProcessDPIAware"));
			if (setDpiAware)
			{
				setDpiAware();
			}
		}

		using GetDpiForSystemFn = UINT (WINAPI *)();
		auto getDpiForSystem = reinterpret_cast<GetDpiForSystemFn>(
			GetProcAddress(user32, "GetDpiForSystem"));
		if (getDpiForSystem)
		{
			return static_cast<float>(getDpiForSystem()) / 96.0f;
		}
	}

	HDC screenDC = GetDC(nullptr);
	const int dpi = screenDC ? GetDeviceCaps(screenDC, LOGPIXELSX) : 96;
	if (screenDC)
	{
		ReleaseDC(nullptr, screenDC);
	}
	return static_cast<float>(dpi) / 96.0f;
}

eKeyCode keyCodeFromVirtualKey(WPARAM vk)
{
	if (vk >= '0' && vk <= '9') return (eKeyCode)(KEY_0 + vk - '0');
	if (vk >= 'A' && vk <= 'Z') return (eKeyCode)(KEY_A + vk - 'A');
	if (vk >= VK_F1 && vk <= VK_F12) return (eKeyCode)(KEY_F1 + vk - VK_F1);

	switch (vk)
	{
		case VK_ESCAPE: return KEY_ESC;
		case VK_SPACE: return KEY_SPACE;
		case VK_BACK: return KEY_BS;
		case VK_TAB: return KEY_TAB;
		case VK_RETURN: return KEY_ENTER;
		case VK_SHIFT: return KEY_SHIFT;
		case VK_CONTROL: return KEY_CTRL;
		case VK_MENU: return KEY_ALT;
		case VK_INSERT: return KEY_INS;
		case VK_DELETE: return KEY_DEL;
		case VK_HOME: return KEY_HOME;
		case VK_END: return KEY_END;
		case VK_PRIOR: return KEY_PAGEUP;
		case VK_NEXT: return KEY_PAGEDOWN;
		case VK_UP: return KEY_UP;
		case VK_DOWN: return KEY_DOWN;
		case VK_LEFT: return KEY_LEFT;
		case VK_RIGHT: return KEY_RIGHT;
		case VK_NUMLOCK: return KEY_NUMLOCK;
		default: return KEY_NULL;
	}
}
}

Win32App::Win32App(const int argc, const char *argv[]) :
	Application(argc, argv),
	m_hDC(nullptr),
	m_hRC(nullptr),
	m_hWnd(nullptr),
	m_hInstance(nullptr),
	m_appName(nullptr),
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
			gfxAPIDraw();
			// OpenGL owns an HGLRC and presents through GDI. Vulkan presents its
			// swapchain in gfxAPIDraw(), so it must not call SwapBuffers here.
			if (m_hRC)
				SwapBuffers(m_hDC);
			Sleep(16);
		}
	}
}


void Win32App::createWindow(const char *title, int x, int y, int width, int height, bool fullscreen)
{
#ifdef _DEBUG
	//RedirectIOToConsole();
#endif

	const float dpiScale = enableHighDpiRendering();
	if (!fullscreen)
	{
		width = static_cast<int>(width * dpiScale + 0.5f);
		height = static_cast<int>(height * dpiScale + 0.5f);
	}
	FI::LOG("Windows high-DPI rendering scale:", dpiScale,
		" physical client target:", width, "x", height);

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
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pParent);
	}  
 	else   
	{      
		pParent = (Win32App*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
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
			if (wParam < 256) m_bKeys[wParam] = true;
			if ((lParam & (1LL << 30)) == 0)
			{
				FI::Event e;
				e.setType(FI::EVENT_KEY_PRESS);
				e.setData((unsigned int)keyCodeFromVirtualKey(wParam), 0);
				setEvent(e);
			}
			return 0;		// Jump Back
		}

		case WM_KEYUP:		// Has A Key Been Released?
		{
			if (wParam < 256) m_bKeys[wParam] = false;
			FI::Event e;
			e.setType(FI::EVENT_KEY_RELEASE);
			e.setData((unsigned int)keyCodeFromVirtualKey(wParam), 0);
			setEvent(e);
			return 0;	// Jump Back
		}

		case WM_SIZE:		// Resize The Direct3D Window
		{
 			// LoWord=Width, HiWord=Height
			resize(LOWORD(lParam), HIWORD(lParam));
			return 0;			// Jump Back
		}
		
		case WM_LBUTTONDOWN:
			SetCapture(m_hWnd);
			{
				FI::Event e; e.setType(FI::EVENT_MOUSE_LEFT_CLICK);
				e.setData((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)); setEvent(e);
			}
			return 0;
			
		case WM_LBUTTONUP:
			ReleaseCapture();
			{
				FI::Event e; e.setType(FI::EVENT_MOUSE_LEFT_RELEASE);
				e.setData((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)); setEvent(e);
			}
			return 0;
			
		case WM_MBUTTONDOWN:
			{
				FI::Event e; e.setType(FI::EVENT_MOUSE_MIDDLE_CLICK);
				e.setData((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)); setEvent(e);
			}
			return 0;
			
		case WM_MBUTTONUP:
			{
				FI::Event e; e.setType(FI::EVENT_MOUSE_MIDDLE_RELEASE);
				e.setData((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)); setEvent(e);
			}
			return 0;
			
		case WM_RBUTTONDOWN:
			{
				FI::Event e; e.setType(FI::EVENT_MOUSE_RIGHT_CLICK);
				e.setData((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)); setEvent(e);
			}
			return 0;
			
		case WM_RBUTTONUP:
			{
				FI::Event e; e.setType(FI::EVENT_MOUSE_RIGHT_RELEASE);
				e.setData((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)); setEvent(e);
			}
			return 0;
			
		case WM_MOUSEMOVE:
			{
				const float x = (float)GET_X_LPARAM(lParam);
				const float y = (float)GET_Y_LPARAM(lParam);
				const float dx = m_hasLastMousePosition ? x - m_lastMouseX : 0.0f;
				const float dy = m_hasLastMousePosition ? y - m_lastMouseY : 0.0f;
				m_hasLastMousePosition = true; m_lastMouseX = x; m_lastMouseY = y;
				FI::Event e;
				if (wParam & MK_LBUTTON) e.setType(FI::EVENT_MOUSE_LEFT_DRAG);
				else if (wParam & MK_MBUTTON) e.setType(FI::EVENT_MOUSE_MIDDLE_DRAG);
				else if (wParam & MK_RBUTTON) e.setType(FI::EVENT_MOUSE_RIGHT_DRAG);
				else e.setType(FI::EVENT_MOUSE_MOVE);
				e.setData(x, y, dx, dy); setEvent(e);
			}
			return 0;

		case WM_MOUSEWHEEL:
		{
			FI::Event e; e.setType(FI::EVENT_MOUSE_WHEEL);
			e.setData((float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA, 0.0f);
			setEvent(e); return 0;
		}
	}

	return DefWindowProc( m_hWnd, uMsg, wParam, lParam);	// Pass All Unhandled Messages To DefWindowProc
}

void Win32App::warpMouseCursorPosition(unsigned int x, unsigned int y)
{
	SetCursorPos((int)x, (int)y);
}

void Win32App::warpMouseCursorPositionInWindow(float x, float y)
{
	POINT point{(LONG)(x * m_width), (LONG)((1.0f - y) * m_height)};
	ClientToScreen(m_hWnd, &point);
	SetCursorPos(point.x, point.y);
	m_lastMouseX = (float)(x * m_width);
	m_lastMouseY = (float)((1.0f - y) * m_height);
}

void Win32App::setRelativeMouseMode(bool enabled)
{
	if (enabled)
	{
		SetCapture(m_hWnd);
		while (ShowCursor(FALSE) >= 0) {}
	}
	else
	{
		ReleaseCapture();
		while (ShowCursor(TRUE) < 0) {}
	}
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
