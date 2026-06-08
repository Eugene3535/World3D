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
    VkInstance instance = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    if (auto it = m_primaryResources.find(VK_OBJECT_TYPE_INSTANCE); it != m_primaryResources.end())
        instance = static_cast<VkInstance>(it->second);

    if (auto it = m_primaryResources.find(VK_OBJECT_TYPE_DEVICE); it != m_primaryResources.end())
        device = static_cast<VkDevice>(it->second);

    if (instance && device)
    {
        if (auto it = m_primaryResources.find(VK_OBJECT_TYPE_SURFACE_KHR); it != m_primaryResources.end())
            vkDestroySurfaceKHR(instance, static_cast<VkSurfaceKHR>(it->second), VK_NULL_HANDLE);

        vkDestroyDevice(device, VK_NULL_HANDLE);
    }

    if (instance)
        vkDestroyInstance(instance, VK_NULL_HANDLE);
}



void* ResourceManager::getObjectByType(VkObjectType type) const noexcept
{
    if (auto it = m_primaryResources.find(type); it != m_primaryResources.end())
        return it->second;

    return VK_NULL_HANDLE;
}


std::string_view ResourceManager::getObjectTypeAsString(VkObjectType type) const noexcept
{
    return magic_enum::enum_name(type);
}


ResourceManager* ResourceManager::getInstance() noexcept
{
    return g_instance;
}