#ifndef BUFFER_HOLDER_HPP
#define BUFFER_HOLDER_HPP

#include <cstring>
#include <vector>
#include <span>

#include "utils/Tools.hpp"
#include "context/Context.hpp"


struct Buffer
{
    VkBuffer       handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    uint32_t       size;
};


struct BufferHolder
{
    template<class T>
    Buffer allocate(std::span<const T> rawData, VkBufferUsageFlagBits flag, VkCommandPool pool) noexcept
    {
        const auto context = vkContext;
        const auto physicalDevice = context->getPhysicalDevice();
        const auto logicalDevice = context->getLogicalDevice();

        Buffer bufferData = { VK_NULL_HANDLE, VK_NULL_HANDLE, static_cast<uint32_t>(rawData.size()) };
        VkDeviceSize bufferSize = sizeof(T) * rawData.size();

        VkDeviceMemory stagingBufferMemory;
        VkBuffer stagingBuffer = vktools::create_buffer(bufferSize, 
                                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                                                        &stagingBufferMemory, 
                                                        logicalDevice, 
                                                        physicalDevice);

        if (!stagingBuffer)
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
        } guard = { stagingBufferMemory, stagingBuffer, logicalDevice }; 

        if (void* ptr; vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &ptr) == VK_SUCCESS)
        {
            memcpy(ptr, rawData.data(), static_cast<size_t>(bufferSize));
            vkUnmapMemory(logicalDevice, stagingBufferMemory);
        }
        else return {};

        if (flag != VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
        {
            bufferData.handle = vktools::create_buffer(bufferSize, 
                                                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | flag, 
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                                       &bufferData.memory, 
                                                       logicalDevice, 
                                                       physicalDevice);
        }
        else
        {
            bufferData.handle = vktools::create_buffer(bufferSize, 
                                                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | flag, 
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                                                       &bufferData.memory, 
                                                       logicalDevice, 
                                                       physicalDevice);
        }

        if (bufferData.handle)
        {
            vktools::copy_buffer(stagingBuffer, bufferData.handle, bufferSize, logicalDevice, pool, context->getQueue());
            m_buffers.push_back(bufferData);

            return bufferData;
        }

        return {};
    }

    void destroy() noexcept;


private:
    std::vector<Buffer> m_buffers;
};

#endif // !BUFFER_HOLDER_HPP