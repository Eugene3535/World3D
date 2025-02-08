#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "opengl/holder/GlResourceHolder.hpp"
#include "opengl/debug/OpenGLDebugger.hpp"

void heightmap_demo(GLFWwindow* window) noexcept;
void path_demo(GLFWwindow* window) noexcept;
void dune_demo(GLFWwindow* window) noexcept;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    GLint width = 1200;
    GLint height = 800;
    GLFWwindow* window = nullptr;
    GlResourceHolder resourceHolder;
    
    if (window = glfwCreateWindow(width, height, "World3D", nullptr, nullptr); window != nullptr)
    {
        glfwMakeContextCurrent(window);

        if (gladLoadGL())
        {
            glfwSetWindowUserPointer(window, static_cast<void*>(&resourceHolder));
            glfwSwapInterval(1);

            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
            {
                glViewport(0, 0, width, height);
            });
        }
        else
        {
            glfwDestroyWindow(window);
            glfwTerminate();

            return -1;
        }
    }
#ifdef DEBUG
    OpenGLDebugger messager;
#endif

    dune_demo(window);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
