#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP

#include <vulkan/vulkan.h>

class VulkanContext
{
public:
    VulkanContext() noexcept;

    bool create() noexcept;
    void destroy() noexcept;

    VkInstance       getInstance()         const noexcept;
    VkPhysicalDevice getPhysicalDevice()   const noexcept;
    VkDevice         getLogicalDevice()    const noexcept;
    VkQueue          getQueue()            const noexcept;
    uint32_t         getQueueFamilyIndex() const noexcept;

    static VulkanContext* getContext() noexcept;

private:
    bool createInstance()  noexcept;
    bool selectVideoCard() noexcept;
    bool createDevice()    noexcept;

    struct
    {
        VkInstance       instance;
        VkPhysicalDevice physicalDevice;
        VkDevice         logicalDevice;
        VkQueue          queue;
        uint32_t         queueFamilyIndex;
    } m_context;
};


#define vkContext VulkanContext::getContext()

#endif // !VULKAN_CONTEXT_HPP