#include "buffers/BufferHolder.hpp"


void BufferHolder::destroy() noexcept
{
    const auto logicalDevice = vkContext->get<VkDevice>();

    for (const auto& data : m_buffers)
    {
        vkDestroyBuffer(logicalDevice, data.handle, nullptr);
        vkFreeMemory(logicalDevice, data.memory, nullptr);
    }
}