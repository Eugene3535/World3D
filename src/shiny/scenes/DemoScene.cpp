#include <SFML/Window/Window.hpp>
#include <SFML/System/Time.hpp>

#include "scenes/DemoScene.hpp"


DemoScene::DemoScene(sf::Window& window) noexcept:
    m_window(window)
{

}


DemoScene::~DemoScene() = default;


bool DemoScene::init() noexcept
{
    return false;
}


void DemoScene::update(const sf::Time& dt) noexcept
{
    
}


void DemoScene::draw() noexcept
{

}