#ifdef _WIN32
#include <windows.h>
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

#include "VulkanApi.hpp"

int main()
{
    const char title[] = "Star Dust";
    const int width = 800;
    const int height = 600;

    VulkanApi app = {};

    if(!app.create(title, width, height))
        return 1;

    int retCode = app.run();

    return retCode;
}