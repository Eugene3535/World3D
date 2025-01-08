#include <type_traits>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.hpp"
#include "OpenGLDebugger.hpp"
#include "Scene.hpp"
#include "RenderWindow.hpp"


const static int SCR_WIDTH = 1200;
const static int SCR_HEIGHT = 800;


static constexpr uint32_t optionToGlCap(RenderWindow::GlOption option) noexcept
{
    switch (option)
    {
        case RenderWindow::GlOption::Blend:                  return GL_BLEND;
        case RenderWindow::GlOption::ClipDistancei:          return GL_CLIP_DISTANCE0;
        case RenderWindow::GlOption::ColorLogicOp:           return GL_COLOR_LOGIC_OP;
        case RenderWindow::GlOption::CullFace:               return GL_CULL_FACE;
        case RenderWindow::GlOption::DepthClamp:             return GL_DEPTH_CLAMP;
        case RenderWindow::GlOption::DepthTest:              return GL_DEPTH_TEST;
        case RenderWindow::GlOption::Dither:                 return GL_DITHER;
        case RenderWindow::GlOption::FramebufferSrgb:        return GL_FRAMEBUFFER_SRGB;
        case RenderWindow::GlOption::LineSmooth:             return GL_LINE_SMOOTH;
        case RenderWindow::GlOption::Multisample:            return GL_MULTISAMPLE;
        case RenderWindow::GlOption::PolygonOffsetFill:      return GL_POLYGON_OFFSET_FILL;
        case RenderWindow::GlOption::PolygonOffsetLine:      return GL_POLYGON_OFFSET_LINE;
        case RenderWindow::GlOption::PolygonOffsetPoint:     return GL_POLYGON_OFFSET_POINT;
        case RenderWindow::GlOption::PolygonSmooth:          return GL_POLYGON_SMOOTH;
        case RenderWindow::GlOption::PrimitiveRestart:       return GL_PRIMITIVE_RESTART;
        case RenderWindow::GlOption::SampleAlphaToCoverage:  return GL_SAMPLE_ALPHA_TO_COVERAGE;
        case RenderWindow::GlOption::SampleAlphaToOne:       return GL_SAMPLE_ALPHA_TO_ONE;
        case RenderWindow::GlOption::SampleCoverage:         return GL_SAMPLE_COVERAGE;
        case RenderWindow::GlOption::ScissorTest:            return GL_SCISSOR_TEST;
        case RenderWindow::GlOption::StencilTest:            return GL_STENCIL_TEST;
        case RenderWindow::GlOption::TextureCubeMapSeamless: return GL_TEXTURE_CUBE_MAP_SEAMLESS;
        case RenderWindow::GlOption::ProgramPointSize:       return GL_PROGRAM_POINT_SIZE;

        default: return 0;
    }
}


RenderWindow::RenderWindow() noexcept:
    m_clearColor(),
    m_handle(nullptr)
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

    if (GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "World3D", nullptr, nullptr); window != nullptr)
    {
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (gladLoadGL())
        {
            m_handle = reinterpret_cast<GLFWwindow*>(window);
            ShaderProgram::initGlUniformFunctions();

            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
            {
                glViewport(0, 0, width, height);
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


RenderWindow::~RenderWindow() noexcept
{
    if(m_handle)
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_handle));

    glfwTerminate();
}


void RenderWindow::addScene(std::unique_ptr<Scene>&& scene) noexcept
{
    m_scenes.push_back(std::move(scene));
}


void RenderWindow::enable(GlOption option) noexcept
{
    glEnable(optionToGlCap(option));
}


void RenderWindow::disable(GlOption option) noexcept
{
    glDisable(optionToGlCap(option));
}


void RenderWindow::setClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept
{
    m_clearColor[0] = static_cast<float>(r) / 255.0f;
    m_clearColor[1] = static_cast<float>(g) / 255.0f;
    m_clearColor[2] = static_cast<float>(b) / 255.0f;
    m_clearColor[3] = static_cast<float>(a) / 255.0f;
}


int RenderWindow::run(void(*func)(void*, int, int)) noexcept
{
    if (!m_handle)
        return -1;

#ifdef DEBUG
    OpenGLDebugger messager;
#endif

    func(m_handle, SCR_WIDTH, SCR_HEIGHT);

	return 0;
}


void RenderWindow::draw() const noexcept
{
    if(m_handle)
    {
        glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto window = reinterpret_cast<GLFWwindow*>(m_handle);

        for(auto& scene : m_scenes)
            scene->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    auto window = reinterpret_cast<GLFWwindow*>(m_handle);

    auto is_key_pressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    if(is_key_pressed(GLFW_KEY_ESCAPE)) 
    {
        glfwSetWindowShouldClose(window, true);
    }
}


void RenderWindow::close() noexcept
{
    glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_handle), true);
}


bool RenderWindow::isOpen() const noexcept
{
    return (m_handle != nullptr) && !glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_handle));
}


void* RenderWindow::getNativeHandle() noexcept
{
    return m_handle;
}