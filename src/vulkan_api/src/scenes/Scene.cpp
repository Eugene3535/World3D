#include "scenes/Scene.hpp"


Scene::Scene(const Scene* parent) noexcept:
    m_parent(parent)
{

}


Scene::~Scene() = default;


bool Scene::create() noexcept
{
    return false;
}   


bool Scene::isRoot() const noexcept
{
    return (m_parent == nullptr);
}