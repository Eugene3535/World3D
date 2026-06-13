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
        if (auto it = m_primaryResources.find(VK_OBJECT_TYPE_COMMAND_POOL); it != m_primaryResources.end())
            vkDestroyCommandPool(device, static_cast<VkCommandPool>(it->second), VK_NULL_HANDLE);

        if (auto it = m_primaryResources.find(VK_OBJECT_TYPE_SURFACE_KHR); it != m_primaryResources.end())
            vkDestroySurfaceKHR(instance, static_cast<VkSurfaceKHR>(it->second), VK_NULL_HANDLE);

        vkDestroyDevice(device, VK_NULL_HANDLE);
    }

    if (instance)
        vkDestroyInstance(instance, VK_NULL_HANDLE);
}



void* ResourceManager::getObjectByType(VkObjectType type) const noexcept
{
    if (isPrimary(type))
    {
        if (auto it = m_primaryResources.find(type); it != m_primaryResources.end())
            return it->second;
    }

    if (auto it = m_secondaryResources.find(type); it != m_secondaryResources.end())
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


bool ResourceManager::isPrimary(VkObjectType type) const noexcept
{
    switch (type)
    {
        case VK_OBJECT_TYPE_UNKNOWN:                      return false;
        case VK_OBJECT_TYPE_INSTANCE:                     return true;
        case VK_OBJECT_TYPE_PHYSICAL_DEVICE:              return true;
        case VK_OBJECT_TYPE_DEVICE:                       return true;
        case VK_OBJECT_TYPE_QUEUE:                        return true;
        case VK_OBJECT_TYPE_SEMAPHORE:                    return false;
        case VK_OBJECT_TYPE_COMMAND_BUFFER:               return false;
        case VK_OBJECT_TYPE_FENCE:                        return false;
        case VK_OBJECT_TYPE_DEVICE_MEMORY:                return false;
        case VK_OBJECT_TYPE_BUFFER:                       return false;
        case VK_OBJECT_TYPE_IMAGE:                        return false;
        case VK_OBJECT_TYPE_IMAGE_VIEW:                   return false;
        case VK_OBJECT_TYPE_SHADER_MODULE:                return false;
        case VK_OBJECT_TYPE_PIPELINE_LAYOUT:              return false;
        case VK_OBJECT_TYPE_PIPELINE:                     return false;
        case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:        return false;
        case VK_OBJECT_TYPE_SAMPLER:                      return false;
        case VK_OBJECT_TYPE_DESCRIPTOR_POOL:              return false;
        case VK_OBJECT_TYPE_DESCRIPTOR_SET:               return false;
        case VK_OBJECT_TYPE_COMMAND_POOL:                 return true;
        case VK_OBJECT_TYPE_SURFACE_KHR:                  return false;
        case VK_OBJECT_TYPE_SWAPCHAIN_KHR:                return false;
        // case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT: return true;
        // case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT: return true;

        default: return false;
    }
}