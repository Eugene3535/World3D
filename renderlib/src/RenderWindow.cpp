#include <type_traits>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.hpp"
#include "OpenGLDebugger.hpp"
#include "RenderWindow.hpp"


const static int SCR_WIDTH = 1200;
const static int SCR_HEIGHT = 800;


RenderWindow::RenderWindow() noexcept:
	m_window(nullptr)
{
    static_assert(std::is_same_v<GLchar, char>, "GLchar and char are not the same type!\n");
    static_assert(std::is_same_v<GLuint, uint32_t>, "GLuint and uint32_t are not the same type!\n");
    static_assert(std::is_same_v<GLenum, uint32_t>, "GLenum and uint32_t are not the same type!\n");
    static_assert(std::is_same_v<GLint, int32_t>, "GLint and int32_t are not the same type!\n");
    static_assert(std::is_same_v<GLsizei, int32_t>, "GLsizei and uint32_t are not the same type!\n");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    if (m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "World3D", nullptr, nullptr))
    {
        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1);

        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int32_t width, int32_t height)
        {
            glViewport(0, 0, width, height);
        });

        if (gladLoadGL())
        {
            ShaderProgram::initGlUniformFunctions();
        }
        else
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