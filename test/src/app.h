#ifndef _APP_H
#define _APP_H

#include "fi/app/oglApp.h"

#ifdef WIN32
#include "application/d3d9App.h"
//#define D3D9
#endif // WIN32

#include "fi/app/threading.h"
#include "fi/app/json.h"

#ifdef D3D9
class MyApp : public D3D9App
#else
class MyApp : public OGLApp
#endif

{
public:
	MyApp(const int argc = 0, const char *argv[] = nullptr);
	virtual ~MyApp();

	// OGLApp overrides
 	void initScene() override;
	void deinitScene() override;
	void drawScene() override;

	// EventListener overrides
	void onKeyPress(FI::Event e) override;
	void onMouseLeftClick(FI::Event e) override;
	void onMouseRightClick(FI::Event e) override;
	
private:

	unsigned int
		m_shaderProgram,
		m_vertexBuffer,
		m_vertexArrayObject;

};

#endif //_APP_H
