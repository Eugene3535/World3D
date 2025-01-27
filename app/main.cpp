#include <memory>

#include "window/RenderWindow.hpp"
#include "camera/perspective/Perspective.hpp"
#include "Heightmap.hpp"

int main()
{
    RenderWindow rw("World3D", 1200, 800);
    rw.hideCursor();

    Context->enable(GlContext::Option::DepthTest);
    std::array<uint32_t, 1> buffer = Context->create<GlBuffer, 1>();

    auto wndSize = rw.getSize();

    UniformBuffer uniformBuffer(buffer[0]);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GlBuffer::Usage::Dynamic);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    std::unique_ptr<Perspective> camera = std::make_unique<Perspective>(uniformBuffer);
    camera->setupProjectionMatrix(45, static_cast<float>(wndSize.x) / static_cast<float>(wndSize.y), 0.1f, 1000.0f);
    
    auto heightmap = std::make_unique<Heightmap>();
    rw.addScene(std::move(heightmap));

    while (rw.isOpen())
    {
        auto cur = rw.getCursorPosition();
        auto pos = rw.getPosition();
        auto siz = rw.getSize();

        pos.x += siz.x >> 1;
        pos.y += siz.y >> 1;

        camera->rotateX((pos.x - cur.x) * 0.125f);
        camera->rotateY((pos.y - cur.y) * 0.125f);

        rw.setCursorPosition(pos.x, pos.y);

        camera->setPosition(30, 3, 30);
        camera->apply(0.01f);

        rw.draw();
    }

    return 0;
}
