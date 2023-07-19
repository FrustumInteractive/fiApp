/*
 * app.h
 *
 *  Created on: Aug 10, 2015
 *      Author: Roger Dass
 *		Copyright Frustum Interactive Inc. - All rights reserved.
 */

#ifndef SRC_APPLICATION_H_
#define SRC_APPLICATION_H_

#include "fi/app/appDefines.h"
#include "fi/app/event.h"
#include "fi/app/fileIO.h"

#include "fi/fiUtil.h"

#include <string>
#include <vector>

class Application : public FI::EventListener
{
public:
	Application(const int argc = 0, const char *argv[] = 0);
	virtual ~Application(){}
	
	void quit() {m_bQuit=true;}
	void requestRender() {m_bRenderRequested = true;}

	// getters
	std::string executiblePath(){return m_executiblePath;}
	std::string documentPath(){return m_documentPath;}
	std::string cmdLineArg(std::string option, bool hasValue = false);
	std::vector<std::string> filesInDirectory( std::string dirPath, std::string ext);
	bool renderOnRefreshEnabled(){ return m_bRenderOnRefresh; }
	bool isRenderRequested(){ return m_bRenderRequested;}

	// setters
	void setExecutiblePath( std::string path ){ m_executiblePath = path;}
	void setDocumentPath( std::string path ){ m_documentPath = path;}
	void setEvent(FI::Event e);
	void setRenderOnRefresh(bool b){ m_bRenderOnRefresh = b;}

	// windows creation
	virtual void createWindow(const char *title, int x, int y, int width, int height, bool fullscreen)=0;
	virtual void createWindowEx( 
					const char *title,
					int x,
					int y,
					int width,
					int height,
					unsigned int mask ){}
					
	virtual void destroyWindow() {}

	virtual void mainloop() {}

	virtual void resize(int width, int height) {}

	virtual void warpMouseCursorPosition(unsigned int x, unsigned int y){}

	virtual void gfxAPIInit() {}
	virtual void gfxAPIDraw() {}
	virtual void gfxAPIDeinit() {}

protected:

	int
		m_width,
		m_height,
		m_xpos,
		m_ypos,
		m_screenWidth,
		m_screenHeight;

	bool
		m_bFullscreen,
		m_bBorderless,
		m_bActive,        // Window Active Flag
		m_bQuit,
		m_bRenderOnRefresh,
		m_bRenderRequested,
		m_bGfxAPIInitialized = false;

	float m_scaleFactor;

	const char *m_title;

	std::string m_executiblePath;	// application executible path
	std::string m_documentPath;		// writeable document path
	std::vector<std::string> m_cmdLineArgs;
};

#endif /* SRC_APPLICATION_H_ */
