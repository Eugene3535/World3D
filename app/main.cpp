#include <memory>
#include <array>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "window/RenderWindow.hpp"
#include "camera/perspective/Perspective.hpp"
#include "Heightmap.hpp"

int main()
{
    RenderWindow rw("World3D", 1200, 800);
    rw.hideCursor();

    glEnable(GL_DEPTH_TEST);

    std::array<uint32_t, 1> buffer = rw.getResourceHolder()->create<GlBuffer, 1>();

    auto wndSize = rw.getSize();

    GlBuffer uniformBuffer(buffer[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_STATIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    std::unique_ptr<Perspective> camera = std::make_unique<Perspective>();
    camera->setupProjectionMatrix(45, static_cast<float>(wndSize.x) / static_cast<float>(wndSize.y), 0.1f, 1000.0f);
    camera->setPosition(30, 3, 30);
    
    auto scene = std::make_unique<Heightmap>(rw.getResourceHolder());
    auto heightmap = scene.get();
    rw.addScene(std::move(scene));

    while (rw.isOpen())
    {
        if(rw.isKeyPressed(Keyboard::W))
            camera->moveForward(10);

        if(rw.isKeyPressed(Keyboard::A))
            camera->moveLeft(10);

        if(rw.isKeyPressed(Keyboard::S))
            camera->moveBackward(10);

        if(rw.isKeyPressed(Keyboard::D))
            camera->moveRight(10);

        auto playerPos = camera->getPosition();
        playerPos.y = heightmap->getHeightInPoint(playerPos.x, playerPos.z) + 1.7f;
        camera->setPosition(playerPos);
        
        auto cur = rw.getCursorPosition();
        auto pos = rw.getPosition();
        auto siz = rw.getSize();

        pos.x += siz.x >> 1;
        pos.y += siz.y >> 1;

        camera->rotateX((pos.x - cur.x) * 0.125f);
        camera->rotateY((pos.y - cur.y) * 0.125f);

        rw.setCursorPosition(pos.x, pos.y);
        camera->apply(0.01f);
        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        rw.draw();
    }

    return 0;
}
