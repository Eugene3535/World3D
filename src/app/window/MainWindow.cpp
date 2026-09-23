#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenGLApi.hpp"
#include "VulkanApi.hpp"
#include "camera/Camera.hpp"
#include "window/MainWindow.hpp"


static bool vulkanAvailable = true;


MainWindow::MainWindow(Camera& camera) noexcept:
    m_glfwWindow(nullptr),
    m_camera(camera)
{

}


MainWindow::~MainWindow()
{
    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}


bool MainWindow::open(int width, int height) noexcept
{
    if (glfwInit() != GLFW_TRUE)
        return false;

    if (vulkanAvailable)
    {
        m_graphicsApi = std::make_unique<VulkanApi>(m_camera);
        vulkanAvailable = m_graphicsApi->createContext();
    }

    if (!vulkanAvailable)
    {
        m_graphicsApi.reset(new OpenGLApi(m_camera));
        vulkanAvailable = false;
    }
    
    if (vulkanAvailable)
    {
        if (!createVulkanApi(width, height))
            return false;
    }
    else
    {
        if (!createOpenGLApi(width, height))
            return false;
    }

    glfwSetWindowUserPointer(m_glfwWindow, static_cast<void*>(this));
    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(m_glfwWindow, width * 0.5, height * 0.5);
    initCallbacks();

    m_graphicsApi->resize(width, height);
    m_data.cursor = { width * 0.5f, height * 0.5f };

    return true;
}


void MainWindow::close() const noexcept
{
    glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
}


void MainWindow::pollEvents() const noexcept
{
    glfwPollEvents();
}


void MainWindow::display() const noexcept
{
    m_graphicsApi->drawFrame();

    if (!vulkanAvailable) // TODO вынести в апи (present для вулкана)
        glfwSwapBuffers(m_glfwWindow);
}


float MainWindow::getElapsedTime() const noexcept
{
    return static_cast<float>(glfwGetTime());
}


ivec2s MainWindow::getSize() const noexcept
{
    int width, height;
	glfwGetWindowSize(m_glfwWindow, &width, &height);

    return { width, height };
}


bool MainWindow::isKeyPressed(int key) const noexcept
{
    return (glfwGetKey(m_glfwWindow, key) == GLFW_PRESS);
}


bool MainWindow::isOpen() const noexcept
{
    return (!glfwWindowShouldClose(m_glfwWindow));
}


bool MainWindow::createOpenGLApi(int width, int height) noexcept
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    if (m_glfwWindow = glfwCreateWindow(width, height, "OpenGL Api", nullptr, nullptr))
    {
        glfwMakeContextCurrent(m_glfwWindow);
        glfwSwapInterval(1);

        if (!m_graphicsApi->createContext())
            return false;

        if (!m_graphicsApi->createView(0))
            return false;

        m_data.cursor = { width * 0.5f, height * 0.5f };

        return true;
    }

    return false;
}


bool MainWindow::createVulkanApi(int width, int height) noexcept
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (m_glfwWindow = glfwCreateWindow(width, height, "Vulkan Api", nullptr, nullptr); !m_glfwWindow)
        return false;

    uint64_t windowHandle = 0;

#ifdef _WIN32
    windowHandle = reinterpret_cast<uint64_t>(glfwGetWin32Window(m_glfwWindow));
#endif

#ifdef __linux__
    windowHandle = reinterpret_cast<uint64_t>(glfwGetX11Window(m_glfwWindow));
#endif

    if (!windowHandle)
        return false;

    if (!m_graphicsApi->createView(windowHandle))
        return false;

    return true;
}


void MainWindow::initCallbacks() noexcept
{
    glfwSetFramebufferSizeCallback(m_glfwWindow, [](GLFWwindow* window, int width, int height) -> void
    {
        if (auto* wnd = static_cast<MainWindow*>(glfwGetWindowUserPointer(window)))
        {
            wnd->m_graphicsApi->resize(width, height);
        }
    });

    glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    });

    glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow* window, double xpos, double ypos) -> void
    {
        if (auto* wnd = static_cast<MainWindow*>(glfwGetWindowUserPointer(window)))
        {
            float xoffset = static_cast<float>(xpos) - wnd->m_data.cursor.x;
            float yoffset = vulkanAvailable ? (static_cast<float>(ypos) - wnd->m_data.cursor.y) : (wnd->m_data.cursor.y - static_cast<float>(ypos));

            wnd->m_data.cursor = { static_cast<float>(xpos), static_cast<float>(ypos) };
            wnd->m_camera.processMouseMovement(xoffset, yoffset);
        }
    });
}