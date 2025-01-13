#include <memory>

#include "RenderWindow.hpp"
#include "Heightmap.hpp"

int main()
{
    RenderWindow rw;
    rw.enable(RenderWindow::GlOption::DepthTest);
    auto heightmap = std::make_unique<Heightmap>(&rw);
    rw.addScene(std::move(heightmap));

    while (rw.isOpen())
    {
        rw.draw();
    }

    return 0;
}
