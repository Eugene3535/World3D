#ifndef DEMO_SCENE_HPP
#define DEMO_SCENE_HPP


namespace sf
{
    class Window;
    class Time;
}


class DemoScene
{
public:
    DemoScene(class sf::Window& window) noexcept;
    virtual ~DemoScene();

    virtual bool init(class GlResourceHolder& holder) noexcept;
    virtual void update(const class sf::Time& dt) noexcept;
    virtual void draw() noexcept;

    bool isLoaded() const noexcept;

protected:
    class sf::Window& m_window;
    bool m_isLoaded;
};


#endif // !DEMO_SCENE_HPP