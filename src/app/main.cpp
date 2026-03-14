#ifdef DEBUG
#include <cstdio>
#endif

#include "VulkanApp.hpp"


int main()
{
	VulkanApp app;
	int retCode = -1;

	if (app.init())
		retCode = app.run();

    return retCode;
}