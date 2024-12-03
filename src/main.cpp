#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Image.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"

void heightmap_demo(GLFWwindow* window, GLint scr_width, GLint scr_height);

int main()
{
    const GLint SCR_WIDTH = 1200;
    const GLint SCR_HEIGHT = 800;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "World3D", nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
    {
        glViewport(0, 0, width, height);
    });

    if (!gladLoadGL())
    {
        glfwTerminate();

        return -1;
    }

    heightmap_demo(window, SCR_WIDTH, SCR_HEIGHT);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}