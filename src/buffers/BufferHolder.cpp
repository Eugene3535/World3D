#include "buffers/BufferHolder.hpp"


void BufferHolder::destroy() noexcept
{
    const auto* logicalDevice = vkContext->get<VulkanDevice>();
    const auto device = logicalDevice->getHandle();

    for (const auto& data : m_buffers)
    {
        vkDestroyBuffer(device, data.handle, nullptr);
        vkFreeMemory(device, data.memory, nullptr);
    }
}