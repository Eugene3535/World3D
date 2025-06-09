

template<class T>
T* SceneManager::pushScene() noexcept
{
    std::string name(typeid(T).name());

    if(auto it = m_scenes.find(name); it != m_scenes.end())
        return static_cast<T*>(it->second.get());

    m_scenes[name] = std::make_shared<T>(m_window);

    return static_cast<T*>(m_scenes[name].get());
}


template<class T> 
T* SceneManager::getScene() noexcept
{
    std::string name(typeid(T).name());

    if(auto it = m_scenes.find(name); it != m_scenes.end())
        return static_cast<T*>(it->second.get());

    return nullptr;
}


template<class T> 
void SceneManager::popScene() noexcept
{
    std::string name(typeid(T).name());
    
    if(auto it = m_scenes.find(name); it != m_scenes.end())
        m_scenes.erase(it);
}