#include <cglm/util.h>

#include "utils/Tools.hpp"

BEGIN_NAMESPACE_VKTOOLS


uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice gpu) noexcept
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    return 0;
}


VkPresentModeKHR SwapChainSupportDetails::getPresentMode() const noexcept
{
    for (const auto mode : presentModes)
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;

    return VK_PRESENT_MODE_FIFO_KHR;
}


VkSurfaceFormatKHR SwapChainSupportDetails::getSurfaceFormat() const noexcept
{
    if(surfaceFormats.empty())
        return {};

    for (const auto& surfaceFmt : surfaceFormats)
        if (surfaceFmt.format == VK_FORMAT_B8G8R8A8_SRGB &&
            surfaceFmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return surfaceFmt;
    
    return surfaceFormats[0];
}


std::shared_ptr<SwapChainSupportDetails> SwapChainSupportDetails::querySupport(VkPhysicalDevice GPU, VkSurfaceKHR surface) noexcept
{
    auto details = std::make_shared<SwapChainSupportDetails>();
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GPU, surface, &details->capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, surface, &formatCount, VK_NULL_HANDLE);

    details->surfaceFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, surface, &formatCount, details->surfaceFormats.data());
    
    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, surface, &presentModeCount, VK_NULL_HANDLE);

    details->presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, surface, &presentModeCount, details->presentModes.data());
    
    return details;
}


VkExtent2D SwapChainSupportDetails::chooseSwapExtent(const SwapChainSupportDetails* details, const VkExtent2D* currentExtent)
{
    VkExtent2D actualExtent = { 0, 0 };

    if (details->capabilities.currentExtent.width != UINT_MAX)
    {
        return details->capabilities.currentExtent;
    }
    else
    {
        memcpy(&actualExtent, currentExtent, sizeof(VkExtent2D));
        actualExtent.width  = glm_clamp(actualExtent.width, details->capabilities.minImageExtent.width, details->capabilities.maxImageExtent.width);
        actualExtent.height = glm_clamp(actualExtent.height, details->capabilities.minImageExtent.height, details->capabilities.maxImageExtent.height);
    }

    return actualExtent;
}


VkCommandBuffer begin_single_time_commands(VkDevice device, VkCommandPool pool) noexcept
{
    const VkCommandBufferAllocateInfo allocInfo = 
    {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = VK_NULL_HANDLE,
        .commandPool        = pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkCommandBuffer cmd;

    if(vkAllocateCommandBuffers(device, &allocInfo, &cmd) != VK_SUCCESS)
        return VK_NULL_HANDLE;

    const VkCommandBufferBeginInfo beginInfo = 
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = VK_NULL_HANDLE,
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = VK_NULL_HANDLE
    };

    if(vkBeginCommandBuffer(cmd, &beginInfo) == VK_SUCCESS)
        return cmd;

    vkFreeCommandBuffers(device, pool, 1, &cmd);

    return VK_NULL_HANDLE;
}


void end_single_time_commands(VkCommandBuffer cmd, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept
{
    vkEndCommandBuffer(cmd);

    const VkSubmitInfo submitInfo = 
    {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = VK_NULL_HANDLE,
        .waitSemaphoreCount   = 0,
        .pWaitSemaphores      = VK_NULL_HANDLE,
        .pWaitDstStageMask    = VK_NULL_HANDLE,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &cmd,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores    = VK_NULL_HANDLE
    };

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(device, pool, 1, &cmd);
}


VkBuffer create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory* bufferMemory, VkDevice device, VkPhysicalDevice gpu) noexcept
{
    const VkBufferCreateInfo bufferInfo = 
    {
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = VK_NULL_HANDLE,
        .flags                 = 0,
        .size                  = size,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = VK_NULL_HANDLE
    };

    VkBuffer buffer;

    if (vkCreateBuffer(device, &bufferInfo, VK_NULL_HANDLE, &buffer) != VK_SUCCESS)
        return VK_NULL_HANDLE;

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    const VkMemoryAllocateInfo allocInfo = 
    {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = VK_NULL_HANDLE,
        .allocationSize  = memRequirements.size,
        .memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties, gpu)
    };

    if (vkAllocateMemory(device, &allocInfo, VK_NULL_HANDLE, bufferMemory) != VK_SUCCESS)
    {
        vkDestroyBuffer(device, buffer, VK_NULL_HANDLE);

        return VK_NULL_HANDLE;
    }
    
    if(vkBindBufferMemory(device, buffer, *bufferMemory, 0) != VK_SUCCESS)
    {
        vkFreeMemory(device, *bufferMemory, VK_NULL_HANDLE);
        vkDestroyBuffer(device, buffer, VK_NULL_HANDLE);

        return VK_NULL_HANDLE;
    }

    return buffer;
}


void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept
{
    VkCommandBuffer cmd = begin_single_time_commands(device, pool);

    if(cmd)
    {
        const VkBufferCopy copyRegion = 
        {
            .srcOffset = 0,
            .dstOffset = 0,
            .size      = size
        };

        vkCmdCopyBuffer(cmd, srcBuffer, dstBuffer, 1, &copyRegion);
        end_single_time_commands(cmd, device, pool, queue);
    }
}


bool transition_image_layout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept
{
    VkCommandBuffer cmd = begin_single_time_commands(device, pool);

    if(cmd)
    {
        VkImageMemoryBarrier barrier = 
        {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext               = VK_NULL_HANDLE,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstAccessMask       = VK_ACCESS_NONE,
            .oldLayout           = oldLayout,
            .newLayout           = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = image,
            .subresourceRange    = 
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1
            }
        };

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_NONE;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_NONE;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            end_single_time_commands(cmd, device, pool, queue);

            return false; // unsupported transition
        } 

        vkCmdPipelineBarrier(
            cmd,
            sourceStage, destinationStage,
            0,
            0, VK_NULL_HANDLE,
            0, VK_NULL_HANDLE,
            1, &barrier);

        end_single_time_commands(cmd, device, pool, queue);

        return true;
    }

    return false;
}


bool copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept
{
    VkCommandBuffer cmd = begin_single_time_commands(device, pool);

    if(cmd)
    {
        const VkBufferImageCopy region = 
        {
            .bufferOffset      = 0,
            .bufferRowLength   = 0,
            .bufferImageHeight = 0,
            .imageSubresource  = 
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = 1
            },
            .imageOffset = 
            {
                .x = 0, 
                .y = 0, 
                .z = 0
            },
            .imageExtent = 
            {
                .width  = width,
                .height = height,
                .depth  = 1
            }
        };

        vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        end_single_time_commands(cmd, device, pool, queue);

        return true;
    }

    return false;
}


bool create_image_2D(
                    VkExtent2D extent, 
                    VkFormat format, 
                    VkImageTiling tiling, 
                    VkImageUsageFlags usage, 
                    VkMemoryPropertyFlags properties, 
                    VkImage* image, 
                    VkDeviceMemory* imageMemory, 
                    VkPhysicalDevice gpu, 
                    VkDevice device) noexcept
{
    bool result = false;

    const VkImageCreateInfo imageInfo = 
    {
        .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext     = VK_NULL_HANDLE,
        .flags     = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format    = format,
        .extent    = 
        {
            .width  = extent.width,
            .height = extent.height,
            .depth  = 1
        },
        .mipLevels             = 1,
        .arrayLayers           = 1,
        .samples               = VK_SAMPLE_COUNT_1_BIT,
        .tiling                = tiling,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = VK_NULL_HANDLE,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED
    };

    result = (vkCreateImage(device, &imageInfo, VK_NULL_HANDLE, image) == VK_SUCCESS);

    if(result)
    {
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, *image, &memRequirements);

        const VkMemoryAllocateInfo allocInfo = 
        {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext           = VK_NULL_HANDLE,
            .allocationSize  = memRequirements.size,
            .memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties, gpu)
        };

        result = (vkAllocateMemory(device, &allocInfo, VK_NULL_HANDLE, imageMemory) == VK_SUCCESS);

        if (result)
            result = (vkBindImageMemory(device, *image, *imageMemory, 0) == VK_SUCCESS);
    }

    return result;
}


bool create_image_view_2D(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView) noexcept
{
    const VkImageViewCreateInfo viewInfo = 
    {
        .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext      = VK_NULL_HANDLE,
        .flags      = 0,
        .image      = image,
        .viewType   = VK_IMAGE_VIEW_TYPE_2D,
        .format     = format,
        .components = 
        {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = 
        {
            .aspectMask     = aspectFlags,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1
        }
    };

    return (vkCreateImageView(device, &viewInfo, VK_NULL_HANDLE, imageView) == VK_SUCCESS);
}


VkFormat find_supported_format(const VkFormat* formats, uint32_t count, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice gpu) noexcept
{
    for (uint32_t i = 0; i < count; ++i)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(gpu, formats[i], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return formats[i];
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return formats[i];
        }
    }

    return VK_FORMAT_UNDEFINED;
}


VkFormat find_depth_format(VkPhysicalDevice gpu) noexcept
{
    const VkFormat formats[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };

    return find_supported_format(formats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, gpu);
}


bool has_stencil_component(VkFormat format) noexcept
{
    return ( (format == VK_FORMAT_D32_SFLOAT_S8_UINT) || (format == VK_FORMAT_D24_UNORM_S8_UINT) );
}

END_NAMESPACE_VKTOOLS