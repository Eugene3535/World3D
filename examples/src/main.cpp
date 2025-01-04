#include "RenderWindow.hpp"

void heightmap_demo(struct GLFWwindow* window, int scr_width, int scr_height);
void path_demo(struct GLFWwindow* window, int scr_width, int scr_height);
void dune_demo(struct GLFWwindow* window, int scr_width, int scr_height);

int main()
{
    return RenderWindow().run(heightmap_demo);
}
