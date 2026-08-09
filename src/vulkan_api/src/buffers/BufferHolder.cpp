#include "buffers/BufferHolder.hpp"


void BufferHolder::destroy() noexcept
{
    const auto logicalDevice = vkContext->get<VkDevice>();

    for(const auto& data : m_buffers)
    {
        vkDestroyBuffer(logicalDevice, data.handle, VK_NULL_HANDLE);
        vkFreeMemory(logicalDevice, data.memory, VK_NULL_HANDLE);
    }
}