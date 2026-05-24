#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP

#include <vulkan/vulkan.h>

class VulkanContext
{
public:
    VulkanContext() noexcept;

    static VulkanContext* getInstance() noexcept;
    
    bool createInstance()  noexcept;
    bool selectVideoCard() noexcept;
    bool createDevice()    noexcept;
    void destroy()         noexcept;

    VkInstance       instance;
    VkPhysicalDevice GPU;
    VkDevice         device;
    VkQueue          queue;
    uint32_t         mainQueueFamilyIndex;
};


#define vkContext VulkanContext::getInstance()

#endif // !VULKAN_CONTEXT_HPP