#include "buffers/BufferHolder.hpp"


void BufferHolder::destroy(VkDevice device) noexcept
{
    for(const auto& data : m_buffers)
    {
        vkDestroyBuffer(device, data.handle, VK_NULL_HANDLE);
        vkFreeMemory(device, data.memory, VK_NULL_HANDLE);
    }
}