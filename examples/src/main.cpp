#include "RenderWindow.hpp"

void heightmap_demo(void* handle, int scr_width, int scr_height);
void path_demo(void* handle, int scr_width, int scr_height);
void dune_demo(void* handle, int scr_width, int scr_height);

int main()
{
    return RenderWindow().run(heightmap_demo);
}
