template<class Callback>
bool ResourceManager::allocateObject(VkObjectType type, Callback&& callback) noexcept
{
    if (isPrimary(type))
    {
        if (auto it = m_primaryResources.find(type); it != m_primaryResources.end())
            return true;
    }

    if (void* object = callback())
    {
        if (isPrimary(type))
        {
            return m_primaryResources.try_emplace(type, object).second;
        }
        else
        {
            return m_secondaryResources.try_emplace(type, object).second;
        }
    }

    return false;
}