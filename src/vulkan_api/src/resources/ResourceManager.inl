template<class Callback>
bool ResourceManager::allocateObject(VkObjectType type, Callback&& callback) noexcept
{
    bool isPrimary = (type == VK_OBJECT_TYPE_INSTANCE)        ||
                     (type == VK_OBJECT_TYPE_PHYSICAL_DEVICE) ||
                     (type == VK_OBJECT_TYPE_DEVICE);

    if (void* object = callback())
    {
        if (isPrimary)
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


template<class T>
T ResourceManager::getObjectByType(VkObjectType type) const noexcept
{
    if (auto it = m_primaryResources.find(type); it != m_primaryResources.end())
        return static_cast<T>(it->second);

    return VK_NULL_HANDLE;
}