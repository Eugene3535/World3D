#include "camera/Camera.hpp"
#include "files/FileProvider.hpp"
#include "loop/MainLoop.hpp"
#include "window/MainWindow.hpp"


int main(int argc, char* argv[])
{
    FileProvider provider(argv[0]);

    const int width  = 800;
    const int height = 600;

    Camera     hud;
    MainWindow window(hud);

    if (!window.open(width, height))
        return 1;

    MainLoop loop(window);
    loop(hud);

    return 0;
}