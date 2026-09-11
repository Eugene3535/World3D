#include <GLFW/glfw3.h>

#include "window/MainWindow.hpp"
#include "camera/Camera.hpp"
#include "loop/MainLoop.hpp"



MainLoop::MainLoop(MainWindow& window) noexcept:
    m_window(window)
{

}


MainLoop::~MainLoop() = default;


void MainLoop::operator()(Camera& camera) noexcept
{
    float lastFrame = 0.f;

    while (m_window.isOpen())
    {
        const float currentFrame = m_window.getElapsedTime();
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		m_window.pollEvents();

        if (m_window.isKeyPressed(GLFW_KEY_W))
            camera.processKeyboard(Camera::Direction::FORWARD, deltaTime);

        if (m_window.isKeyPressed(GLFW_KEY_S))
            camera.processKeyboard(Camera::Direction::BACKWARD, deltaTime);

        if (m_window.isKeyPressed(GLFW_KEY_A))
            camera.processKeyboard(Camera::Direction::LEFT, deltaTime);

        if (m_window.isKeyPressed(GLFW_KEY_D))
            camera.processKeyboard(Camera::Direction::RIGHT, deltaTime);

        m_window.display();
    }
}