/*
* mobileApp.h
*
* Created on: Aug 10, 2015
*	Author: Roger Dass
*	Copyright Frustum Interactive Inc. - All rights reserved.
*/

#ifndef _MOBILEAPP_H
#define _MOBILEAPP_H

#include "app.h"

class MobileApp : public Application
{
public:
	MobileApp(const int argc = 0, const char *argv[] = 0);
	virtual ~MobileApp();

	void createWindow(const char *title, int x, int y, int width, int height, bool fullscreen) override;
	void destroyWindow() override;

	void mainloop() override;

protected:
	void swapBuffers();

};

#endif /*_MOBILEAPP_H*/
