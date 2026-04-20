#ifdef DEBUG
#include <cstdio>
#endif

#include "MainWindow.hpp"


int main()
{
	const char title[] = "Star Dust";
    const int width = 800;
    const int height = 600;

	MainWindow app;
	int retCode = -1;

	if (app.create(title, width, height))
		retCode = app.run();

    return retCode;
}