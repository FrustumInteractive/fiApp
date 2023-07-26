/*
* x11App.h
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#ifndef _X11APP_H
#define _X11APP_H

#include <X11/Xlib.h>
#include <GL/glx.h>
#include "app.h"

class X11App : public Application
{
public:
	X11App( const int argc = 0, const char *argv[] = 0 );
	virtual ~X11App();

	void createWindow(const char *title, int x, int y, int width, int height, bool fullscreen) override;
	void createWindowEx( 
					const char *title,
					int x,
					int y,
					int width,
					int height,
					unsigned int mask ) override;

	void destroyWindow() override;

	void mainloop() override;

	void resize(int x, int y) override {}

	void warpMouseCursorPosition(unsigned int x, unsigned int y) override;


protected:
	
	void swapBuffers();

	Display					*m_display;
	Window					m_rootWindow;
	GLint					*m_attributes;
	XVisualInfo				*m_visualInfo;
	Colormap				m_colormap;
	XSetWindowAttributes	m_setWindowAttributes;
	Window					m_window;
	GLXContext				m_glContext;
	XWindowAttributes		m_windowAttributes;
	XEvent					m_xEvent;
	GLXFBConfig				m_bestFbc; //best frame buffer cfg

	int m_glxMinor;
	int m_glxMajor;

	int m_usecondsSinceLastDisplay = 0;
};

#endif /*_X11APP_H*/
