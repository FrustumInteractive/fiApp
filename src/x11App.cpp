/*
* x11App.cpp
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include "fi/app/x11App.h"
#include "fi/debug/trace.h"

typedef struct {
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
} Hints;

static float x11ContentScale(Display *display)
{
	const char *overrideScale = getenv("FI_X11_SCALE");
	if (overrideScale && *overrideScale)
	{
		char *end = nullptr;
		const float scale = strtof(overrideScale, &end);
		if (end != overrideScale && *end == '\0' && std::isfinite(scale) && scale > 0.0f)
			return scale;
		FI::LOG("Ignoring invalid FI_X11_SCALE:", overrideScale);
	}

	float resourceScale = 1.0f;
	XrmInitialize();
	const char *resourceString = XResourceManagerString(display);
	if (resourceString)
	{
		XrmDatabase database = XrmGetStringDatabase(resourceString);
		if (database)
		{
			char *type = nullptr;
			XrmValue value{};
			if (XrmGetResource(database, "Xft.dpi", "Xft.Dpi", &type, &value) && value.addr)
			{
				char *end = nullptr;
				const float dpi = strtof(value.addr, &end);
				if (end != value.addr && std::isfinite(dpi) && dpi > 0.0f)
					resourceScale = dpi / 96.0f;
			}
			XrmDestroyDatabase(database);
		}
	}

	return resourceScale;
}

X11App::X11App(const int argc, const char *argv[]) :
	Application(argc,argv),
	m_display(0),
	m_rootWindow(0),
	m_attributes(0),
	m_visualInfo(0),
	m_colormap(0),
	m_window(0),
	m_glContext(0),
	m_bestFbc(0),
	m_glxMinor(0),
	m_glxMajor(0)
{
}

X11App::~X11App()
{
}

void X11App::createWindow(const char *title, int x, int y, int width, int height, bool fullscreen)
{
	unsigned int msk = 0;
	m_bFullscreen = fullscreen;
	if(m_bFullscreen) {
		msk = WND_FULLSCREEN | WND_BORDERLESS;
	}

	createWindowEx( title, x, y, width, height, msk );
}

void X11App::createWindowEx(const char *title, int x, int y, int width, int height, unsigned int mask)
{
	m_xpos = x;
	m_ypos = y;
	m_width = width;
	m_height = height;
	m_title = title;

	m_bFullscreen = (mask & WND_FULLSCREEN) == WND_FULLSCREEN;
	m_bBorderless = (mask & WND_BORDERLESS) == WND_BORDERLESS;

	unsigned long cwmask = CWColormap | CWEventMask; //masks for X11 window properties

	m_display = XOpenDisplay(NULL);
	if(m_display == NULL) {
		FI::LOG("Cannot connect to X server");
		 exit(0);
	}

	m_scaleFactor = x11ContentScale(m_display);
	FI::LOG("X11 content scale:", m_scaleFactor);

	m_rootWindow = DefaultRootWindow(m_display);
	int screenID = DefaultScreen(m_display); //Get the default screen id

#if defined(FI_GFX_VULKAN)
	// Vulkan presents directly to an X11 window. Do not select the window's
	// visual through GLX: GLX framebuffer configuration (including its sample
	// count) is an OpenGL concern and is not used by a Vulkan swapchain.
	XVisualInfo visualTemplate{};
	visualTemplate.visualid = XVisualIDFromVisual(DefaultVisual(m_display, screenID));
	int visualCount = 0;
	m_visualInfo = XGetVisualInfo(m_display, VisualIDMask, &visualTemplate, &visualCount);
	if (!m_visualInfo || visualCount < 1)
	{
		FI::LOG("Failed to retrieve the default X11 visual for Vulkan");
		exit(1);
	}
#else
	// FBConfigs were added in GLX version 1.3.
	if (!glXQueryVersion(m_display, &m_glxMajor, &m_glxMinor) || ((m_glxMajor==1) && (m_glxMinor<2)) || (m_glxMajor<1))
	{
		FI::LOG("Invalid GLX version:", m_glxMajor, m_glxMinor);
		exit(1);
	}

	// - VISUAL INFO -
	static int att[] =
	{
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		//GLX_SAMPLE_BUFFERS  , 1,
		//GLX_SAMPLES         , 4,
 		None
	};

	static int att_simple[] =
	{
		GLX_RGBA,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 0,
		GLX_DEPTH_SIZE,	24,
		GLX_DOUBLEBUFFER,
		None
	};

	if(m_glxMinor == 2)
	{
		m_attributes = att_simple;
		m_visualInfo = glXChooseVisual(m_display, screenID, m_attributes);
		if(!m_visualInfo) {
			FI::LOG("no appropriate visual found");
			exit(0);
		}
		else {
			FI::LOG("visual selected: ", (void *)m_visualInfo->visualid);
		}
	}
	else
	{
		m_attributes = att;
		// Getting matching framebuffer configs
		int fbcount;
		GLXFBConfig* fbc = glXChooseFBConfig(m_display, DefaultScreen(m_display), m_attributes, &fbcount);
		if (!fbc)
		{
				FI::LOG( "Failed to retrieve a framebuffer configuration" );
				exit(1);
		}
		FI::LOG( "Found", fbcount, "matching FB configs." );	

		// Prefer 4x MSAA. If it is unavailable, choose the closest supported
		// sample count, preferring the lower count when equally close.
		FI::LOG("Getting XVisualInfos" );
		int preferred_fbc = -1;
		int preferred_samples = 0;
		int preferred_distance = 999;

		int i;
		for (i=0; i<fbcount; ++i)
		{
				XVisualInfo *vi = glXGetVisualFromFBConfig( m_display, fbc[i] );
				if ( vi )
				{
					int samp_buf, samples;
					glXGetFBConfigAttrib( m_display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
					glXGetFBConfigAttrib( m_display, fbc[i], GLX_SAMPLES       , &samples  );
		
					FI::LOG("Matching fbconfig:", i, "visual ID:", vi->visualid, "SAMPLE_BUFFERS:", samp_buf,"SAMPLES:", samples );

					const int effectiveSamples = samp_buf ? samples : 0;
					const int distance = std::abs(effectiveSamples - 4);
					if (preferred_fbc < 0 || distance < preferred_distance ||
						(distance == preferred_distance && effectiveSamples < preferred_samples))
					{
						preferred_fbc = i;
						preferred_samples = effectiveSamples;
						preferred_distance = distance;
					}
				}
				XFree( vi );
		}

		m_bestFbc = fbc[ preferred_fbc ];

		// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
		XFree( fbc );

		// Get a visual
		m_visualInfo = glXGetVisualFromFBConfig( m_display, m_bestFbc );
	}
#endif

	FI::LOG("Chosen visual ID:", m_visualInfo->visualid );

	m_colormap = XCreateColormap(m_display, m_rootWindow, m_visualInfo->visual, AllocNone);

	m_setWindowAttributes.colormap = m_colormap;
	m_setWindowAttributes.border_pixel = 0;
	m_setWindowAttributes.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
		PointerMotionMask | ButtonMotionMask | Button1MotionMask | StructureNotifyMask;

	// FULLSCREEN
	if(m_bFullscreen)
	{
		Screen *scrn = ScreenOfDisplay(m_display, screenID); //this takes all of the surface (of all monitors combined)
		m_width = scrn->width;
		m_height = scrn->height;
		m_xpos = 0;
		m_ypos = 0;

		m_setWindowAttributes.override_redirect = True;
		cwmask |= CWBorderPixel | CWOverrideRedirect;
	}

	m_window = XCreateWindow(
					m_display,
					m_rootWindow,
					m_xpos, m_ypos,
					m_width, m_height,
					0,
					m_visualInfo->depth,
					InputOutput,
					m_visualInfo->visual,
					cwmask,
					&m_setWindowAttributes );

	XMapWindow(m_display, m_window);
	XStoreName( m_display, m_window, m_title);

	if(m_bBorderless)
	{
		if(m_bFullscreen)
		{
			/*
			int defaultScreen = 0;
			int modeCount = 0;
			XF86VidModeModeInfo **modeLines,
								*modeLine;
			XF86VidModeGetAllModeLines( m_display, defaultScreen, &modeCount, &modeLines );
			for( int i=0; i<modeCount; i++) {
				modeLine = modelines[i];
				FI_DEBUG("mode:", i, "x:", modeLine->hdisplay, "y:", modeLine->vdisplay);
			}
			exit(0); 
			XF86VidModeSwitchToMode(m_display,defaultscreen,video_mode);
			XF86VidModeSetViewPort(m_display,defaultScreen,0,0);
			XMoveResizeWindow(m_display, m_window, 0, 0, m_width, m_height);
			XMapRaised(m_display, m_window);
			XGrabPointer(m_display, m_window, True, 0, GrabModeAsync, GrabModeAsync, m_window, 0L, CurrentTime);
			XGrabKeyboard(m_display, m_window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
			*/
			XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, 0, 0);
			XMapRaised(m_display, m_window);	//window on top
			XGrabKeyboard(m_display, m_window, True, GrabModeAsync, GrabModeAsync, CurrentTime);
			XGrabPointer(m_display, m_window, True,
				ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask,
				GrabModeAsync, GrabModeAsync, m_window, None, CurrentTime);
		}
		Hints hints;
		Atom property;
		hints.flags = 2;        // Specify that we're changing the window decorations.
		hints.decorations = 0;  // 0 (false) means no window decorations.
		property = XInternAtom( m_display, "_MOTIF_WM_HINTS", True);
		XChangeProperty(m_display, m_window, property, property, 32, PropModeReplace, (unsigned char *)&hints, 5);
	}

	gfxAPIInit();
}

void X11App::destroyWindow()
{
	gfxAPIDeinit();
	XDestroyWindow(m_display, m_window );
	XCloseDisplay(m_display);
}

void X11App::mainloop()
{
	bool hasLastMousePosition = false;
	float lastMouseX = 0.0f;
	float lastMouseY = 0.0f;

	while (!m_bQuit)
	{
		while (XCheckTypedWindowEvent(m_display, m_window, ConfigureNotify, &m_xEvent))
		{
			const int width = m_xEvent.xconfigure.width;
			const int height = m_xEvent.xconfigure.height;
			if (width > 0 && height > 0 && (width != m_width || height != m_height))
			{
				resize(width, height);
			}
		}

		if (XCheckWindowEvent(m_display, m_window, KeyPressMask | KeyReleaseMask, &m_xEvent))
		{
			XEvent nextEvent;
			if (XCheckWindowEvent(m_display, m_window, KeyPressMask | KeyReleaseMask, &nextEvent))
			{
				if (m_xEvent.xkey.serial == nextEvent.xkey.serial)
				{
					// discard these two events and continue the loop. Same serial ID is a sign that pressed/release pair is auto-gen'd
					continue;
				}
			}
			auto ks = XkbKeycodeToKeysym(m_display, m_xEvent.xkey.keycode, 0, 0);
			FI::Event e;
			eKeyCode kc;

			switch (m_xEvent.type)
			{
				case KeyPress:
					e.setType(FI::EVENT_KEY_PRESS); break;
				case KeyRelease:
					e.setType(FI::EVENT_KEY_RELEASE); break;
			}

			// map X11 Keysym values to our own
			if (ks >= XK_0 && ks <= XK_9)
			{
				kc = (eKeyCode)((int)KEY_0 + ks - XK_0);
			}
			else if (ks >= XK_A && ks <= XK_Z)
			{
				kc = (eKeyCode)((int)KEY_A + ks - XK_A);
			}
			else if (ks >= XK_a && ks <= XK_z)
			{
				kc = (eKeyCode)((int)KEY_A + ks - XK_a);
			}
			else
			{
				switch (ks)
				{
					case XK_F1: kc=KEY_F1; break;
					case XK_Escape: kc=KEY_ESC; break;
					case XK_Down: kc=KEY_DOWN; break;
					case XK_Up: kc=KEY_UP; break;
					case XK_Left: kc=KEY_LEFT; break;
					case XK_Right: kc=KEY_RIGHT; break;
					case XK_Return: kc=KEY_ENTER; break;
					case XK_space: kc=KEY_SPACE; break;
				}
			}

			e.setData((unsigned int)kc, 0);
			setEvent(e);
		}


		if(XCheckWindowEvent(m_display, m_window, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask, &m_xEvent))
		{
			FI::Event e;
			const float inputScale = m_scaleFactor > 0.0f ? m_scaleFactor : 1.0f;

			switch(m_xEvent.type)
			{
				case ButtonPress:
					hasLastMousePosition = true;
					lastMouseX = (float)m_xEvent.xbutton.x / inputScale;
					lastMouseY = (float)m_xEvent.xbutton.y / inputScale;
					switch(m_xEvent.xbutton.button)
					{
						case Button1:
							e.setType(FI::EVENT_MOUSE_LEFT_CLICK);
							e.setData(lastMouseX, lastMouseY);
							break;
						case Button3:
							e.setType(FI::EVENT_MOUSE_RIGHT_CLICK);
							e.setData(lastMouseX, lastMouseY);
							break;
						default:
							break;
					}
					break;

				case ButtonRelease:
					hasLastMousePosition = true;
					lastMouseX = (float)m_xEvent.xbutton.x / inputScale;
					lastMouseY = (float)m_xEvent.xbutton.y / inputScale;
					switch(m_xEvent.xbutton.button)
					{
						case Button1:
							e.setType(FI::EVENT_MOUSE_LEFT_RELEASE);
							e.setData(lastMouseX, lastMouseY);
							break;
						case Button3:
							e.setType(FI::EVENT_MOUSE_RIGHT_RELEASE);
							e.setData(lastMouseX, lastMouseY);
							break;
						default:
							break;
					}
					break;

				case MotionNotify:
				{
					const float mouseX = (float)m_xEvent.xmotion.x / inputScale;
					const float mouseY = (float)m_xEvent.xmotion.y / inputScale;
					const float mouseDx = hasLastMousePosition ? mouseX - lastMouseX : 0.0f;
					const float mouseDy = hasLastMousePosition ? mouseY - lastMouseY : 0.0f;
					hasLastMousePosition = true;
					lastMouseX = mouseX;
					lastMouseY = mouseY;

					if(m_xEvent.xmotion.state & Button1MotionMask)
					{
						e.setType(FI::EVENT_MOUSE_LEFT_DRAG);
						e.setData(mouseX, mouseY, mouseDx, mouseDy);
						setEvent(e);
					}
					else if(m_xEvent.xmotion.state & Button2MotionMask)
					{
						e.setType(FI::EVENT_MOUSE_MIDDLE_DRAG);
						e.setData(mouseX, mouseY, mouseDx, mouseDy);
						setEvent(e);
					}
					else if(m_xEvent.xmotion.state & Button3MotionMask)
					{
						e.setType(FI::EVENT_MOUSE_RIGHT_DRAG);
						e.setData(mouseX, mouseY, mouseDx, mouseDy);
						setEvent(e);
					}

					e.setType(FI::EVENT_MOUSE_MOVE);
					e.setData(mouseX, mouseY, mouseDx, mouseDy);
					break;
				}

				default:
					break;
			}

			setEvent(e);
		}


		m_usecondsSinceLastDisplay += m_timeToNextInputUpdate*1000.0f;
		if (m_usecondsSinceLastDisplay > m_timeToNextDisplayUpdate*1000.0f)
		{
			XSync(m_display, true); //discard event queue (needed to make mouse response immediate)
			gfxAPIDraw();
			// Vulkan presents its swapchain from gfxAPIDraw(). All other native
			// Linux builds retain the original GLX presentation path.
#if !defined(FI_GFX_VULKAN)
			glXSwapBuffers(m_display, m_window);
#endif
			m_usecondsSinceLastDisplay = 0;
		}
		usleep(m_timeToNextInputUpdate*1000.0f);
	}
}

void X11App::warpMouseCursorPosition(unsigned int x, unsigned int y)
{
	XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, x, y);
}

void X11App::warpMouseCursorPositionInWindow(float x, float y)
{
	if (x < 0.0f) x = 0.0f;
	if (x > 1.0f) x = 1.0f;
	if (y < 0.0f) y = 0.0f;
	if (y > 1.0f) y = 1.0f;

	const unsigned int localX = (unsigned int)std::lround(x * (float)m_width);
	const unsigned int localY = (unsigned int)std::lround((1.0f - y) * (float)m_height);
	XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, localX, localY);
}

void X11App::swapBuffers()
{
	glXSwapBuffers(m_display, m_window);
}
