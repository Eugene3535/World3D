#include <cstdio>

#include <glad/glad.h>

#include <SFML/Window.hpp>

#ifdef _WIN32
#include <windows.h>
extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

int heightmap_demo(sf::Window& window) noexcept;
int path_demo(sf::Window& window) noexcept;
int dune_demo(sf::Window& window) noexcept;
int platformer_demo(sf::Window& window) noexcept;
int orbit_demo(sf::Window& window) noexcept;

int main()
{
    sf::ContextSettings settings;
    settings.majorVersion = 4;
    settings.minorVersion = 6;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.attributeFlags = sf::ContextSettings::Core;

    const uint32_t width = 1200;
    const uint32_t height = 800;

    sf::Window window(sf::VideoMode(width, height), "World3D", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    if (!gladLoadGL()) 
    {
        printf("Failed to initialize GLAD\n");

        return -1;
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    
#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    glDebugMessageCallback( [](GLenum source,
                               GLenum type,
                               GLuint id,
                               GLenum severity,
                               GLsizei length,
                               const GLchar* message,
                               const void* userParam )
        {
            fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
        }, nullptr );
#endif

    int returnValue = platformer_demo(window);

    return returnValue;
}
