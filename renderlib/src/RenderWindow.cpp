#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGLDebugger.hpp"
#include "RenderWindow.hpp"


const static int SCR_WIDTH = 1200;
const static int SCR_HEIGHT = 800;


RenderWindow::RenderWindow() noexcept:
	m_window(nullptr)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "World3D", nullptr, nullptr);

    if (m_window)
    {
        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1);

        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int32_t width, int32_t height)
        {
            glViewport(0, 0, width, height);
        });

        if (!gladLoadGL())
        {
            glfwDestroyWindow(m_window);
            glfwTerminate();
            m_window = nullptr;
        }
    }
}

RenderWindow::~RenderWindow() noexcept
{
    if(m_window)
        glfwDestroyWindow(m_window);

    glfwTerminate();
}

int RenderWindow::run(void(*func)(GLFWwindow*, int, int)) noexcept
{
    if (!m_window)
        return -1;

#ifdef DEBUG
    OpenGLDebugger messager;
#endif

    func(m_window, SCR_WIDTH, SCR_HEIGHT);

	return 0;
}