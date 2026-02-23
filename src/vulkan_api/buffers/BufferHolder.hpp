#ifndef BUFFER_HOLDER_HPP
#define BUFFER_HOLDER_HPP

#include <vector>
#include <span>

#include "vulkan_api/utils/Tools.hpp"
#include "vulkan_api/context/Context.hpp"


struct VK_API Buffer
{
    VkBuffer handle = VK_NULL_HANDLE;
    uint32_t size = 0;
};


struct VK_API BufferHolder
{
    template<class T>
    Buffer allocate(std::span<const T> rawData, VkBufferUsageFlagBits flag, const VulkanContext* context, VkCommandPool pool) noexcept
    {
        BufferHolder::Data bufferData = { VK_NULL_HANDLE, VK_NULL_HANDLE, static_cast<uint32_t>(rawData.size()) };
        VkDeviceSize bufferSize = sizeof(T) * rawData.size();

        VkDeviceMemory stagingBufferMemory;
        VkBuffer stagingBuffer = vktools::create_buffer(
                                                        bufferSize, 
                                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                                                        &stagingBufferMemory, 
                                                        context->device, 
                                                        context->GPU);

        if(!stagingBuffer)
            return {};

        struct BufferMemoryDeleter
        {
            ~BufferMemoryDeleter() 
            {
                vkDestroyBuffer(device, buffer, VK_NULL_HANDLE);
                vkFreeMemory(device, memory, VK_NULL_HANDLE);
            }

            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkBuffer buffer       = VK_NULL_HANDLE;
            VkDevice device       = VK_NULL_HANDLE;
        } guard = { stagingBufferMemory, stagingBuffer, context->device }; 

        if (void* ptr; vkMapMemory(context->device, stagingBufferMemory, 0, bufferSize, 0, &ptr) == VK_SUCCESS)
        {
            memcpy(ptr, rawData.data(), static_cast<size_t>(bufferSize));
            vkUnmapMemory(context->device, stagingBufferMemory);
        }
        else return {};

        bufferData.handle = vktools::create_buffer(
                                                   bufferSize, 
                                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | flag, 
                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                                   &bufferData.memory, 
                                                   context->device, 
                                                   context->GPU);

        if(bufferData.handle)
        {
            vktools::copy_buffer(stagingBuffer, bufferData.handle, bufferSize, context->device, pool, context->queue);
            m_buffers.push_back(bufferData);

            return { bufferData.handle, bufferData.size };
        }

        return {};
    }

    void destroy(VkDevice device) noexcept;

    struct VK_API Data
    {
        VkBuffer       handle = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        uint32_t       size;
    };

private:
    std::vector<Data> m_buffers;
};

#endif // !BUFFER_HOLDER_HPP