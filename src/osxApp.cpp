#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <chrono>

#include "fi/app/osxApp.h"

#include <mach-o/dyld.h>
#include <unistd.h>

using namespace std;

static OSXApp *sLiveResizeApp = nullptr;
static int sLastAppliedResizeW = -1;
static int sLastAppliedResizeH = -1;
static bool sDrawInProgress = false;

static bool OSXGetLatestWindowSize(int &winW, int &winH)
{
	bool sawResize = false;
	winW = 0;
	winH = 0;
	while (CWConsumeResizeEvent(&winW, &winH))
	{
		sawResize = true;
	}
	if (!sawResize)
	{
		// Fallback: poll in case notification/coalescing misses a tick.
		CWGetWindowSize(winW, winH);
	}
	return sawResize;
}

static bool OSXApplyResizeIfNeeded(OSXApp *app, bool onlyIfChanged)
{
	if (app == nullptr)
	{
		return false;
	}

	int winW = 0;
	int winH = 0;
	OSXGetLatestWindowSize(winW, winH);

#if FI_GFX_METAL
	const int drawW = winW;
	const int drawH = winH;
#else
	const float sf = app->scaleFactor();
	const int drawW = (int)(winW * sf);
	const int drawH = (int)(winH * sf);
#endif

	if (drawW <= 0 || drawH <= 0)
	{
		return false;
	}

	if (onlyIfChanged && drawW == sLastAppliedResizeW && drawH == sLastAppliedResizeH)
	{
		return false;
	}

	sLastAppliedResizeW = drawW;
	sLastAppliedResizeH = drawH;
	app->resize(drawW, drawH);
	return true;
}

static void OSXLiveResizeDrawCallback()
{
	if (sLiveResizeApp == nullptr)
	{
		return;
	}
	if (sDrawInProgress)
	{
		return;
	}
	sDrawInProgress = true;
	OSXApplyResizeIfNeeded(sLiveResizeApp, true);

	sLiveResizeApp->gfxAPIDraw();
	CWSwapBuffers();
	sDrawInProgress = false;
}

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
	sLiveResizeApp = this;
	sLastAppliedResizeW = -1;
	sLastAppliedResizeH = -1;
	CWSetLiveResizeDrawCallback(OSXLiveResizeDrawCallback);
#if !FI_GFX_METAL
	CWSetVSync(m_bVsyncEnabled);
	FI::LOG("OSXApp OpenGL vsync:", m_bVsyncEnabled ? "enabled" : "disabled");
#endif
#if FI_GFX_METAL
    // For Metal/Vulkan, treat m_width/m_height as drawable pixel size.
    // CWGetWindowSizeC returns drawable pixel size.
    CWGetWindowSize(m_width, m_height);
#else
	cout << "content scale factor: " << m_scaleFactor << endl;
	m_width = width*m_scaleFactor; // set render surface to be right backing size irrespective of 'DPI'
	m_height = height*m_scaleFactor; // application->event coordinate normalization uses scale factor
#endif

	// get display res
	CWGetScreenSize(m_screenWidth, m_screenHeight);

	gfxAPIInit();
}

void OSXApp::destroyWindow()
{
	CWSetLiveResizeDrawCallback(nullptr);
	sLiveResizeApp = nullptr;
	sLastAppliedResizeW = -1;
	sLastAppliedResizeH = -1;
	gfxAPIDeinit();
}

void OSXApp::mainloop()
{
	bool bLeftBtnDown = false;
	bool bMiddleBtnDown = false;
	bool bRightBtnDown = false;
#if !FI_GFX_METAL
	using Clock = std::chrono::steady_clock;
	auto swapWindowStart = Clock::now();
	double swapWindowSeconds = 0.0;
	double swapBlockSeconds = 0.0;
	double maxFrameSeconds = 0.0;
	unsigned int slowFrames = 0;
	unsigned int swapFrames = 0;
#endif

	while(!m_bQuit) 
	{
		if(CWCheckQuitMessage())
			return;
			
		CWPollDevice();		// handle events

		// Keep resize sync in lockstep with Cocoa even when no drawRect callback fires.
		OSXApplyResizeIfNeeded(this, true);
		
		int lb, mb, rb, mx, my;
		float mdx = 0.0f;
		float mdy = 0.0f;
		int mevent = CWGetMouseEvent(&lb, &mb, &rb, &mx, &my, &mdx, &mdy);
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
			e.setType(FI::EVENT_MOUSE_MIDDLE_CLICK);
			e.setData((float)mx, (float)my);
			setEvent(e);
			bMiddleBtnDown = true;
			break;

			case MOUSEEVENT_MBUTTONUP:
				e.setType(FI::EVENT_MOUSE_MIDDLE_RELEASE);
				e.setData((float)mx, (float)my);
				setEvent(e);
				bMiddleBtnDown = false;
				break;

			case MOUSEEVENT_MOVE:
				if (bLeftBtnDown)
				{
					e.setType(FI::EVENT_MOUSE_LEFT_DRAG);
					e.setData((float)mx, (float)my, mdx, mdy);
				}
				else if (bMiddleBtnDown)
				{
					e.setType(FI::EVENT_MOUSE_MIDDLE_DRAG);
					e.setData((float)mx, (float)my, mdx, mdy);
				}
				else if (bRightBtnDown)
				{
					e.setType(FI::EVENT_MOUSE_RIGHT_DRAG);
					e.setData((float)mx, (float)my, mdx, mdy);
				}
				else
				{
					e.setType(FI::EVENT_MOUSE_MOVE);
					e.setData((float)mx, (float)my, mdx, mdy);
				}
				setEvent(e);
				break;
			default:
				break;
		}

		if (eKeyCode kc = CWInkey())
		{
			if (kc == KEY_WHEELUP || kc == KEY_WHEELDOWN)
			{
				e.setType(FI::EVENT_MOUSE_WHEEL);
				const float wheelDelta = (kc == KEY_WHEELUP) ? 1.0f : -1.0f;
				e.setData(wheelDelta, 0.0f);
			}
			else
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
			}
			setEvent(e);
		}

			if (!sDrawInProgress)
			{
				sDrawInProgress = true;
#if !FI_GFX_METAL
			if (m_bVsyncEnabled)
			{
				CWWaitForDisplayRefresh();
			}
			auto frameBegin = Clock::now();
#endif
			gfxAPIDraw();		// our draw call
#if !FI_GFX_METAL
			auto swapBegin = Clock::now();
#endif
			CWSwapBuffers();
#if !FI_GFX_METAL
			auto swapEnd = Clock::now();
			const double frameSeconds = std::chrono::duration<double>(swapEnd - frameBegin).count();
			if (frameSeconds > maxFrameSeconds)
			{
				maxFrameSeconds = frameSeconds;
			}
			if (frameSeconds > 1.0 / 30.0)
			{
				slowFrames++;
			}
			swapFrames++;
			swapBlockSeconds += std::chrono::duration<double>(swapEnd - swapBegin).count();
			swapWindowSeconds = std::chrono::duration<double>(swapEnd - swapWindowStart).count();

			if (swapWindowSeconds >= 5.0 && swapFrames > 0)
			{
				double swapFps = (double)swapFrames / swapWindowSeconds;
				double avgSwapBlockMs = (swapBlockSeconds / (double)swapFrames) * 1000.0;
				FI::LOG(
					"frame loop fps (",
					swapWindowSeconds,
					"s):",
					swapFps,
					" avg swap block (ms):",
					avgSwapBlockMs,
					" max frame (ms):",
					maxFrameSeconds * 1000.0,
					" slow frames >33ms:",
					slowFrames);
				swapWindowStart = swapEnd;
				swapWindowSeconds = 0.0;
				swapBlockSeconds = 0.0;
				maxFrameSeconds = 0.0;
				slowFrames = 0;
				swapFrames = 0;
			}
#endif
				sDrawInProgress = false;
			}
			//CWSleep(17);
		}
}

void OSXApp::warpMouseCursorPosition(unsigned x, unsigned y)
{
	CWWarpMouseCursorPosition(x, y);
}

void OSXApp::warpMouseCursorPositionInWindow(float x, float y)
{
	if (m_scaleFactor <= 0.0f)
	{
		return;
	}

	if (x < 0.0f) x = 0.0f;
	if (x > 1.0f) x = 1.0f;
	if (y < 0.0f) y = 0.0f;
	if (y > 1.0f) y = 1.0f;

	const unsigned localX = (unsigned)(x * (float)m_width / m_scaleFactor);
	const unsigned localY = (unsigned)((1.0f - y) * (float)m_height / m_scaleFactor);
	CWWarpMouseCursorPositionInWindow(localX, localY);
}

void OSXApp::swapBuffers()
{
	CWSwapBuffers();
}
