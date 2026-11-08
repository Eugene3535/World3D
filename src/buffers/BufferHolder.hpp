#pragma once

#include <cstring>
#include <span>
#include <vector>

#include "context/Context.hpp"
#include "utils/Tools.hpp"


struct Buffer
{
    VkBuffer       handle = nullptr;
    VkDeviceMemory memory = nullptr;
    uint32_t       size;
};


struct BufferHolder
{
    template <class T> // Fast memory
    Buffer allocate(std::span<const T> rawData, VkBufferUsageFlagBits flag, VkCommandPool pool) noexcept
    {
        const auto context        = vkContext;
        const auto physicalDevice = context->get<VkPhysicalDevice>();
        const auto logicalDevice  = context->get<VkDevice>();

        Buffer       bufferData = {nullptr, nullptr, static_cast<uint32_t>(rawData.size())};
        VkDeviceSize bufferSize = rawData.size_bytes();

        VkDeviceMemory stagingBufferMemory;
        VkBuffer       stagingBuffer = vktools::create_buffer(bufferSize,
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
                vkDestroyBuffer(device, buffer, nullptr);
                vkFreeMemory(device, memory, nullptr);
            }

            VkDeviceMemory memory = nullptr;
            VkBuffer       buffer = nullptr;
            VkDevice       device = nullptr;
        } guard = {stagingBufferMemory, stagingBuffer, logicalDevice};

        if (void* ptr; vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &ptr) == VK_SUCCESS)
        {
            memcpy(ptr, rawData.data(), static_cast<size_t>(bufferSize));
            vkUnmapMemory(logicalDevice, stagingBufferMemory);
        }
        else
            return {};

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
            vktools::copy_buffer(stagingBuffer, bufferData.handle, bufferSize, logicalDevice, pool, context->get<VkQueue>());
            m_buffers.push_back(bufferData);

            return bufferData;
        }

        return {};
    }


    template <class T> // Slow memory, but may be mapped
    Buffer create(std::span<const T> rawData, VkBufferUsageFlags usage) noexcept
    {
        const auto context       = vkContext;
        const auto logicalDevice = context->get<VkDevice>();

        const VkBufferCreateInfo bufferInfo =
            {.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
             .pNext                 = nullptr,
             .flags                 = 0,
             .size                  = rawData.size_bytes(),
             .usage                 = usage,
             .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
             .queueFamilyIndexCount = 0,
             .pQueueFamilyIndices   = nullptr};

        VkBuffer vertexBuffer;

        if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
            return {};

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(logicalDevice, vertexBuffer, &memRequirements);

        const VkMemoryAllocateInfo allocInfo =
            {.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
             .pNext           = nullptr,
             .allocationSize  = memRequirements.size,
             .memoryTypeIndex = vktools::find_memory_type(memRequirements.memoryTypeBits,
                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                          vkContext->get<VkPhysicalDevice>())};

        VkDeviceMemory vertexBufferMemory;

        if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
        {
            vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);

            return {};
        }

        vkBindBufferMemory(logicalDevice, vertexBuffer, vertexBufferMemory, 0);

        void* data;
        vkMapMemory(logicalDevice, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
        memcpy(data, rawData.data(), bufferInfo.size);
        vkUnmapMemory(logicalDevice, vertexBufferMemory);

        m_buffers.push_back({vertexBuffer, vertexBufferMemory, static_cast<uint32_t>(rawData.size())});

        return {vertexBuffer, vertexBufferMemory, static_cast<uint32_t>(rawData.size())};
    }

    void destroy() noexcept;

private:
    std::vector<Buffer> m_buffers;
};