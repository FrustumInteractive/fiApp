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
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include "fi/app/x11App.h"
#include "fi/debug/trace.h"

typedef struct {
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
} Hints;

X11App::X11App(const int argc, const char *argv[]) :
	Application(argc,argv),
	m_display(0),
	m_attributes(0),
	m_visualInfo(0)
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

	m_rootWindow = DefaultRootWindow(m_display);
	int screenID = DefaultScreen(m_display); //Get the default screen id

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

		// Pick the FB config/visual with the most samples per pixel
  		FI::LOG("Getting XVisualInfos" );
  		int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

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

      				if ( best_fbc < 0 || (samp_buf && samples > best_num_samp) )
        				best_fbc = i, best_num_samp = samples;
      				if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
        				worst_fbc = i, worst_num_samp = samples;
    			}
    			XFree( vi );
  		}

  		m_bestFbc = fbc[ best_fbc ];

  		// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
  		XFree( fbc );

  		// Get a visual
  		m_visualInfo = glXGetVisualFromFBConfig( m_display, m_bestFbc );
  	}

	FI::LOG("Chosen visual ID:", m_visualInfo->visualid );

	m_colormap = XCreateColormap(m_display, m_rootWindow, m_visualInfo->visual, AllocNone);

	m_setWindowAttributes.colormap = m_colormap;
	m_setWindowAttributes.border_pixel = 0;
	m_setWindowAttributes.event_mask = 
		ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | 
		PointerMotionMask | ButtonMotionMask | Button1MotionMask;

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
		hints.decorations = 0;  // 0 (false) means that window decorations should go bye-bye.
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
	while(!m_bQuit) 
	{
		if(XCheckWindowEvent(m_display, m_window, KeyPressMask, &m_xEvent))
		{
			char *key_string = XKeysymToString(XkbKeycodeToKeysym(m_display, m_xEvent.xkey.keycode, 0, 0));
			FI::Event e;
			eKeyCode kc;
			e.setType(FI::EVENT_KEY_PRESS);

			if(strncmp(key_string, "Left", 4) == 0) kc=KEY_LEFT;
			else if(strncmp(key_string, "Right", 5) == 0) kc=KEY_RIGHT;
			else if(strncmp(key_string, "Up", 2) == 0) kc=KEY_UP;
			else if(strncmp(key_string, "Down", 4) == 0) kc=KEY_DOWN;
			else if(strncmp(key_string, "F1", 2) == 0) kc=KEY_F1;
			else if(strncmp(key_string, "Escape", 5) == 0) kc=KEY_ESC;
			else if(strncmp(key_string, "q", 1) == 0) kc=KEY_Q;

			e.setData((unsigned int)kc,0);
			setEvent(e);
		}


		if(XCheckWindowEvent(m_display,	m_window, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask, &m_xEvent)) 
		{
			FI::Event e;

			switch(m_xEvent.type)
			{
				case ButtonPress:
					switch(m_xEvent.xbutton.button) {
						case Button1:
							e.setType(FI::EVENT_MOUSE_LEFT_CLICK);
							e.setData((float)m_xEvent.xbutton.x, (float)m_xEvent.xbutton.y);
							break;
						case Button3:
							e.setType(FI::EVENT_MOUSE_RIGHT_CLICK);
							e.setData((float)m_xEvent.xbutton.x, (float)m_xEvent.xbutton.y);
							break;
						default:
							break;
					}
					break;

				case ButtonRelease:
					switch(m_xEvent.xbutton.button) {
						case Button1:
							e.setType(FI::EVENT_MOUSE_LEFT_RELEASE);
							e.setData((float)m_xEvent.xbutton.x, (float)m_xEvent.xbutton.y);
							break;
						case Button3:
							e.setType(FI::EVENT_MOUSE_RIGHT_RELEASE);
							e.setData((float)m_xEvent.xbutton.x, (float)m_xEvent.xbutton.y);
							break;
						default:
							break;
					}
					break;

				case MotionNotify:
					if(m_xEvent.xmotion.state & Button1MotionMask) {
						e.setType(FI::EVENT_MOUSE_LEFT_DRAG);
						e.setData((float)m_xEvent.xmotion.x, (float)m_xEvent.xmotion.y);
						setEvent(e);
					}
					else if(m_xEvent.xmotion.state & Button2MotionMask) {
						e.setType(FI::EVENT_MOUSE_MIDDLE_DRAG);
						e.setData((float)m_xEvent.xmotion.x, (float)m_xEvent.xmotion.y);
						setEvent(e);
					}
					else if(m_xEvent.xmotion.state & Button3MotionMask) {
						e.setType(FI::EVENT_MOUSE_RIGHT_DRAG);
						e.setData((float)m_xEvent.xmotion.x, (float)m_xEvent.xmotion.y);
						setEvent(e);
					}

					e.setType(FI::EVENT_MOUSE_MOVE);
					e.setData((float)m_xEvent.xmotion.x, (float)m_xEvent.xmotion.y);
					break;
					
				default:
					break;
			}

			setEvent(e);
		}

		XSync(m_display, true); //discard event queue (needed to make mouse response immediate)
		gfxAPIDraw();
		glXSwapBuffers(m_display, m_window);
		usleep(16667);
	}
}

void X11App::swapBuffers()
{
	glXSwapBuffers(m_display, m_window);
}
