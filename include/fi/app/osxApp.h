#ifndef _OSXAPP_H
#define _OSXAPP_H

#include "app.h"
#include "cocoaWrapper.h"

class OSXApp : public Application
{
public:
	OSXApp(const int argc = 0, const char *argv[] = 0);
	virtual ~OSXApp();

	void createWindow(const char *title, int x, int y, int width, int height, bool fullscreen) override;
	void destroyWindow() override;

	void warpMouseCursorPosition(unsigned x, unsigned y);

	void mainloop() override;

protected:
	
	void swapBuffers();

	int	*m_attributes;
};

#endif /*_OSXAPP_H*/
