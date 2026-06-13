template<class T>
bool Scene::addScene() noexcept
{
    auto& scene = m_scenes.emplace_back(std::make_unique<T>(this));

    if (!scene->create())
    {
        m_scenes.pop_back();

        return false;
    }

    return true;
}