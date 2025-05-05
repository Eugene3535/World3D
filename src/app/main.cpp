#ifdef DEBUG
#include <cstdio>
#endif

#ifdef _WIN32
#include <windows.h>
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

#include <glad/glad.h>

#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "resources/holder/GlResourceHolder.hpp"
#include "scenes/dune/DuneDemo.hpp"
#include "scenes/fonts/FontDemo.hpp"
#include "scenes/heightmap/HeightmapDemo.hpp"
#include "scenes/orbit/OrbitDemo.hpp"
#include "scenes/paths/PathDemo.hpp"
#include "scenes/platformer/PlatformerDemo.hpp"
#include "scenes/SceneManager.hpp"


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
#ifdef DEBUG
        printf("Failed to initialize GLAD\n");
#endif
        return -1;
    }

#ifdef DEBUG
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    
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

    GlResourceHolder resources;
    SceneManager sceneManager(window);

    // if(!sceneManager.pushScene<DuneDemo>()->init(resources))       return -1;
    if(!sceneManager.pushScene<FontDemo>()->init(resources))       return -1;
    // if(!sceneManager.pushScene<HeightmapDemo>()->init(resources))  return -1;
    // if(!sceneManager.pushScene<OrbitDemo>()->init(resources))      return -1;
    // if(!sceneManager.pushScene<PathDemo>()->init(resources))       return -1;
    // if(!sceneManager.pushScene<PlatformerDemo>()->init(resources)) return -1;

    DemoScene* scene = sceneManager.getScene<FontDemo>();

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        float mouseScrollDelta = 0;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if(event.type == sf::Event::Resized)
                glViewport(0, 0, width, height);

            if(event.type == sf::Event::KeyPressed)
                if(event.key.code == sf::Keyboard::Key::Escape)
                    window.close();

            if(event.type == sf::Event::MouseWheelScrolled)
                mouseScrollDelta = event.mouseWheelScroll.delta; 
        }

        auto dt = clock.restart();

        if(auto* orbit_demo = dynamic_cast<OrbitDemo*>(scene); orbit_demo != nullptr)
            orbit_demo->processMouseScroll(mouseScrollDelta);

        scene->update(dt);
        scene->draw();
        window.display();
    }

    return 0;
}
