#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "data/AppData.hpp"
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
    AppData appData;
    
    if (window = glfwCreateWindow(width, height, "World3D", nullptr, nullptr); window != nullptr)
    {
        glfwMakeContextCurrent(window);

        if (gladLoadGL())
        {
            glfwSetWindowUserPointer(window, static_cast<void*>(&appData));
            glfwSwapInterval(1);

            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
            {
                glViewport(0, 0, width, height);

                if (auto appData = static_cast<AppData*>(glfwGetWindowUserPointer(window)); appData)
                {
                    appData->camera.orthogonal.setupProjectionMatrix(width, height);
                    appData->camera.perspective.setupProjectionMatrix(45, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
                }
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
