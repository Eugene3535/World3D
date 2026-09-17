#pragma once

#include <memory>

#include <cglm/struct/vec2.h>

class MainWindow
{
public:
    MainWindow(class Camera& camera) noexcept;
    ~MainWindow();

    bool open(int width, int height) noexcept;
    void close() const noexcept;

    void pollEvents() const noexcept;
    void display() const noexcept;

    float getElapsedTime() const noexcept;
    ivec2s getSize() const noexcept;

    bool isKeyPressed(int key) const noexcept;
    bool isOpen() const noexcept;

private:
    bool createOpenGLApi(int width, int height) noexcept;
    bool createVulkanApi(int width, int height) noexcept;
    void initCallbacks() noexcept;

    struct GLFWwindow* m_glfwWindow;
    class Camera& m_camera;
	std::unique_ptr<class GraphicsApi> m_graphicsApi;

    struct WindowData
    {
        vec2s cursor;
    } m_data;
};