#include <SFML/Window/Window.hpp>

#include "scenes/DemoScene.hpp"
#include "scenes/SceneManager.hpp"

SceneManager::SceneManager(sf::Window& window) noexcept:
    m_window(window)
{

}

SceneManager::~SceneManager()
{

}