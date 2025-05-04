#ifndef DEMO_SCENE_HPP
#define DEMO_SCENE_HPP

#include "Export.hpp"


namespace sf
{
    class Window;
    class Time;
}


class SHINY_API DemoScene
{
public:
    DemoScene(class sf::Window& window) noexcept;
    virtual ~DemoScene();

    virtual bool init(class GlResourceHolder& holder) noexcept;
    virtual void update(const class sf::Time& dt) noexcept;
    virtual void draw() noexcept;

protected:
    class sf::Window& m_window;
};


#endif // !DEMO_SCENE_HPP