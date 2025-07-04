#ifdef DEBUG
#include <cstdio>
#endif

#ifdef _WIN32
#include <windows.h>
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

#include <glad/glad.h>
#include <SFML/Window/VideoMode.hpp>
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
#include "scenes/light/LightDemo.hpp"
#include "scenes/SceneManager.hpp"


int main()
{
    sf::ContextSettings settings;
    settings.majorVersion = 4;
    settings.minorVersion = 6;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = 4;
    settings.attributeFlags = sf::ContextSettings::Core;

    const uint32_t width = 1200;
    const uint32_t height = 800;

    sf::Window window(sf::VideoMode({width, height}), "World3D", sf::Style::Default, sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);

    if (!gladLoadGL()) 
        return -1;
    
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
                               const void* userParam)
        {
            fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
        }, nullptr );
#endif

    GlResourceHolder resources;
    SceneManager sceneManager(window);

//  DuneDemo, FontDemo, HeightmapDemo, LightDemo, OrbitDemo, PathDemo, PlatformerDemo
    if(!sceneManager.pushScene<LightDemo>()->init(resources))          return -1;
    DemoScene* scene = sceneManager.getScene<LightDemo>();

    sf::Clock clock;

    sf::Vector2i mouseOldPos = sf::Mouse::getPosition(window);

    while(window.isOpen())
    {
        float mouseScrollDelta = 0;
        float mouseMovementDeltaX = 0;
        float mouseMovementDeltaY = 0;

        while(const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }

            if (const auto* resized = event->getIf<sf::Event::Resized>())
                glViewport(0, 0, resized->size.x, resized->size.y);

            if(const auto* scrolled = event->getIf<sf::Event::MouseWheelScrolled>())
                mouseScrollDelta = scrolled->delta;

            if(const auto* mouse_moved = event->getIf<sf::Event::MouseMoved>())
            {
                mouseMovementDeltaX = mouse_moved->position.x - mouseOldPos.x;
                mouseMovementDeltaY = mouse_moved->position.y - mouseOldPos.y;
                mouseOldPos = mouse_moved->position;
            }
        }

        auto dt = clock.restart();

        if(auto* light_demo = dynamic_cast<LightDemo*>(scene); light_demo != nullptr)
        {
            light_demo->processMouseScroll(mouseScrollDelta);
            light_demo->processMouseMovement(mouseMovementDeltaX, mouseMovementDeltaY);
        }

        scene->update(dt);
        scene->draw();
        window.display();
    }

    return 0;
}
