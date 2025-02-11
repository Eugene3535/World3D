#include <cstdio>

#include <glad/glad.h>

#include <SFML/Window.hpp>

#include "data/AppData.hpp"
#include "opengl/debug/OpenGLDebugger.hpp"

int heightmap_demo(sf::Window& window, AppData& data) noexcept;
int path_demo(sf::Window& window, AppData& data) noexcept;
int dune_demo(sf::Window& window, AppData& data) noexcept;

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
    AppData appData;

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
    OpenGLDebugger messager;
#endif

    int returnValue = dune_demo(window, appData);

    return returnValue;
}
