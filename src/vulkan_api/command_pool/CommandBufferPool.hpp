#ifndef COMMAND_BUFFER_POOL_HPP
#define COMMAND_BUFFER_POOL_HPP

#include <array>

#include "vulkan_api/utils/Tools.hpp"


struct VK_API CommandBufferPool
{
    bool create(VkDevice device, uint32_t queueFamilyIndex) noexcept;
    void destroy(VkDevice device) noexcept;

    VkCommandPool handle = VK_NULL_HANDLE;
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> commandBuffers = { VK_NULL_HANDLE, VK_NULL_HANDLE };
};


#endif // !COMMAND_BUFFER_POOL_HPP