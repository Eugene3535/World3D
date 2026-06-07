#include <cassert>

#include <magic_enum/magic_enum.hpp>

#include "resources/ResourceManager.hpp"


static ResourceManager* g_instance;


ResourceManager::ResourceManager() noexcept
{
    assert(g_instance == nullptr);
    g_instance = this;
}


ResourceManager::~ResourceManager()
{
    VkDevice device = VK_NULL_HANDLE;

    if (auto it = m_primaryResources.find(VK_OBJECT_TYPE_DEVICE); it != m_primaryResources.end())
        device = static_cast<VkDevice>(it->second);

    if (device)
    {
        vkDestroyDevice(device, VK_NULL_HANDLE);
    }

    if (auto it = m_primaryResources.find(VK_OBJECT_TYPE_INSTANCE); it != m_primaryResources.end())
        vkDestroyInstance(static_cast<VkInstance>(it->second), VK_NULL_HANDLE);
}


std::string_view ResourceManager::getObjectTypeAsString(VkObjectType type) const noexcept
{
    return magic_enum::enum_name(type);
}


ResourceManager* ResourceManager::getInstance() noexcept
{
    return g_instance;
}