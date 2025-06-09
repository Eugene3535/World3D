#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <typeinfo>

#include "Export.hpp"


class SHINY_API SceneManager final
{
public:
    SceneManager(class sf::Window& window) noexcept;
    ~SceneManager();

    template<class T>
    T* pushScene() noexcept;

    template<class T> 
    T* getScene() noexcept;

    template<class T> 
    void popScene() noexcept;

private:
    class sf::Window& m_window;
    std::unordered_map<std::string, std::shared_ptr<class DemoScene>> m_scenes;
};

#include "scenes/SceneManager.inl"

#endif // !SCENE_MANAGER_HPP