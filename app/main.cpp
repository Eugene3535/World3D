#include <memory>

#include "window/RenderWindow.hpp"
#include "Heightmap.hpp"

int main()
{
    RenderWindow rw("World3D", 1200, 800);
    Context->enable(GlContext::Option::DepthTest);
    
    auto heightmap = std::make_unique<Heightmap>(&rw);
    rw.addScene(std::move(heightmap));

    while (rw.isOpen())
    {
        rw.draw();
    }

    return 0;
}
