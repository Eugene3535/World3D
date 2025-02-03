#include <type_traits>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "opengl/debug/OpenGLDebugger.hpp"
#include "Scene.hpp"
#include "window/RenderWindow.hpp"


RenderWindow::RenderWindow(const char* title, int32_t width, int32_t height) noexcept:
    BaseWindow(title, width, height),
    m_clearColor()
{
    static_assert(std::is_same_v<GLchar, char>, "GLchar and char are not the same type!\n");
    static_assert(std::is_same_v<GLuint, uint32_t>, "GLuint and uint32_t are not the same type!\n");
    static_assert(std::is_same_v<GLenum, uint32_t>, "GLenum and uint32_t are not the same type!\n");
    static_assert(std::is_same_v<GLint, int32_t>, "GLint and int32_t are not the same type!\n");
    static_assert(std::is_same_v<GLsizei, int32_t>, "GLsizei and uint32_t are not the same type!\n");

    if(isOpen())
    {
        ShaderProgram::initGlUniformFunctions();
#ifdef DEBUG
        OpenGLDebugger messager;
#endif
    }   
}


RenderWindow::~RenderWindow() noexcept = default;


void RenderWindow::addScene(std::unique_ptr<Scene>&& scene) noexcept
{
    m_scenes.push_back(std::move(scene));
}


void RenderWindow::setClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept
{
    m_clearColor[0] = static_cast<float>(r) / 255.0f;
    m_clearColor[1] = static_cast<float>(g) / 255.0f;
    m_clearColor[2] = static_cast<float>(b) / 255.0f;
    m_clearColor[3] = static_cast<float>(a) / 255.0f;
}


void RenderWindow::draw() const noexcept
{
    auto window = static_cast<GLFWwindow*>(m_handle);

    auto is_key_pressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    if(m_handle)
    {
        glfwPollEvents();

        if(is_key_pressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto& scene : m_scenes)
            scene->draw();

        glfwSwapBuffers(window);
    }
}