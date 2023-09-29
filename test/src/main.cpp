/************************************************ 
 *	3D GFX Application - Roger Dass c 2015		*
 *		Main.cpp								*
 *												*
 ************************************************/

#include "app.h"

int main( const int argc, const char *argv[] )
{
	FI::LOG("Graphics API Test Application");

	MyApp app(argc, argv);

	app.createWindow("GFX App", 25, 25, 640, 480, false );

#ifndef WEB
	app.mainloop( );
#endif

	FI::LOG("main loop terminated");

	app.destroyWindow();

	return 0;
}
