#include "app.h"

int main(const int argc, const char *argv[])
{
	FI::LOG("fiApp Vulkan Test Application");

	MyApp app(argc, argv);
	app.createWindow("fiApp Vulkan Test", 25, 25, 640, 480, false);
	app.mainloop();
	app.destroyWindow();

	return 0;
}
