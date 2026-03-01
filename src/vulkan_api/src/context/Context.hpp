#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP

#include <vulkan/vulkan.h>

struct VulkanContext
{
    bool create()  noexcept;
    void destroy() noexcept;

    VkInstance       instance             = nullptr;
    VkPhysicalDevice GPU                  = nullptr;
    VkDevice         device               = nullptr;
    VkQueue          queue                = nullptr;
    uint32_t         mainQueueFamilyIndex = 0;
};

#endif // !VULKAN_CONTEXT_HPP