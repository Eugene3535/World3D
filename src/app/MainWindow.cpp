#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "MainWindow.hpp"

// TODO remove magic numbers
static float lastX = 400;
static float lastY = 300;


MainWindow::MainWindow() noexcept:
    m_window(nullptr)
{

}


MainWindow::~MainWindow()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}


bool MainWindow::create(const char* title, int32_t width, int32_t height) noexcept
{
    if(!m_api.createContext())
        return false;

    if (glfwInit() == GLFW_TRUE)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        if (m_window = glfwCreateWindow(width, height, title, nullptr, nullptr))
        {
            glfwSetWindowUserPointer(m_window, static_cast<void*>(&m_api));
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            initCallbacks();
        }
    }

    uint64_t windowHandle = 0;

#ifdef _WIN32
    windowHandle = reinterpret_cast<uint64_t>(glfwGetWin32Window(m_window));
#endif

#ifdef __linux__
    windowHandle = reinterpret_cast<uint64_t>(glfwGetX11Window(m_window));
#endif

    if (!windowHandle)
        return false;

    if (!m_api.createMainView(windowHandle))
        return false;

    if (!m_api.init())
        return false;

    m_api.resize(width, height);

    return true;
}


int MainWindow::run() noexcept
{
    float deltaTime = 0.f;
    float lastFrame = 0.f;

    while (!glfwWindowShouldClose(m_window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window, true);

        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
            m_api.processKeyboard(0, deltaTime);

        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
            m_api.processKeyboard(1, deltaTime);

        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
            m_api.processKeyboard(2, deltaTime);

        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
            m_api.processKeyboard(3, deltaTime);

        m_api.drawFrame();

        glfwPollEvents();
    }

    return 0;
}


void MainWindow::initCallbacks() noexcept
{
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) -> void
    {
        if (auto api = static_cast<VulkanApi*>(glfwGetWindowUserPointer(window)))
        {
            api->resize(width, height);
        }
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xposIn, double yposIn) -> void
    {
        if (auto api = static_cast<VulkanApi*>(glfwGetWindowUserPointer(window)))
        {
            float xpos = (float)xposIn;
            float ypos = (float)yposIn;

            float xoffset = xpos - lastX;
            float yoffset = ypos - lastY;

            lastX = xpos;
            lastY = ypos;

            api->processMouseMovement(xoffset, yoffset);
        }
    });
}