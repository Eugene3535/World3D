#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/BaseWindow.hpp"


BaseWindow::BaseWindow(const char* title, int32_t width, int32_t height) noexcept:
    m_handle(nullptr)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    if (GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr); window != nullptr)
    {
        glfwMakeContextCurrent(window);

        if (Context->isLoaded())
        {
            m_handle = static_cast<void*>(window);
            glfwSetWindowUserPointer(window, static_cast<void*>(this));

            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
            {
                glViewport(0, 0, width, height);
            });

            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                {
                    if (key == GLFW_KEY_ESCAPE)
                        glfwSetWindowShouldClose(window, GLFW_TRUE);                
                }
                    
            });
        }
        else
        {
            glfwDestroyWindow(window);
            glfwTerminate();
            m_handle = nullptr;
        }
    }
}


BaseWindow::~BaseWindow() noexcept
{
    if(m_handle)
        glfwDestroyWindow(static_cast<GLFWwindow*>(m_handle));
    glfwTerminate();
}


void BaseWindow::close() noexcept
{
    glfwSetWindowShouldClose(static_cast<GLFWwindow*>(m_handle), GLFW_TRUE);
}


void BaseWindow::setVerticalSyncEnabled(bool enabled) noexcept
{
    glfwSwapInterval(enabled ? 1 : 0);
}


void BaseWindow::setCursorPosition(int32_t x, int32_t y) noexcept
{
    glfwSetCursorPos(static_cast<GLFWwindow*>(m_handle), x, y);
}


glm::i32vec2 BaseWindow::getCursorPosition() const noexcept
{
    double xpos, ypos;
    glfwGetCursorPos(static_cast<GLFWwindow*>(m_handle), &xpos, &ypos);

    return { static_cast<int32_t>(xpos), static_cast<int32_t>(ypos) };
}


void BaseWindow::hideCursor() noexcept
{
    glfwSetInputMode(static_cast<GLFWwindow*>(m_handle), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void BaseWindow::showCursor() noexcept
{
    glfwSetInputMode(static_cast<GLFWwindow*>(m_handle), GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}


glm::i32vec2 BaseWindow::getPosition() const noexcept
{
    int32_t xt, yt;
    glfwGetWindowPos(static_cast<GLFWwindow*>(m_handle), &xt, &yt);

    return { static_cast<int32_t>(xt), static_cast<int32_t>(yt) };
}


glm::i32vec2 BaseWindow::getSize() const noexcept
{
    int32_t width, height;
    glfwGetWindowSize(static_cast<GLFWwindow*>(m_handle), &width, &height);

    return { static_cast<int32_t>(width), static_cast<int32_t>(height) };
}


void* BaseWindow::getGLFWHandle() noexcept
{
    return m_handle;
}


bool BaseWindow::isOpen() const noexcept
{
    return (m_handle != nullptr) && !glfwWindowShouldClose(static_cast<GLFWwindow*>(m_handle));
}