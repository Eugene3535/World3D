#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "utils/Tools.hpp"
#include "context/Context.hpp"
#include "texture/Texture2D.hpp"


namespace
{
    struct StbImage
    {
        StbImage(const char* filepath, int mode) noexcept:
            pixels(nullptr), width(0), height(0), channels(0)
        {
            pixels = stbi_load(filepath, &width, &height, &channels, mode);
        }

        ~StbImage()
        {
            stbi_image_free(pixels);
        }

        stbi_uc* pixels;
        int32_t  width;
        int32_t  height;
        int32_t  channels;
    };


    struct BufferMemoryDeleter
    {
        ~BufferMemoryDeleter() 
        {
            if (buffer) vkDestroyBuffer(device, buffer, nullptr);
            if (memory) vkFreeMemory(device, memory, nullptr);
        }

        VkDeviceMemory memory = nullptr;
        VkBuffer buffer = nullptr;
        VkDevice device = nullptr;
    };


    bool create_sampler(Texture2D* texture, VkPhysicalDevice gpu, VkDevice device) noexcept;
}



bool Texture2D::loadFromFile(const char* filepath, VkCommandPool pool) noexcept
{
    const auto context = vkContext;
    const auto physicalDevice = vkContext->getPhysicalDevice(); 
    const auto logicalDevice = vkContext->getLogicalDevice();
    const auto queue = vkContext->getQueue();

    StbImage stbImage(filepath, STBI_rgb_alpha);

    if ( ! stbImage.pixels )
        return false;

    VkDeviceSize imageSize = stbImage.width * stbImage.height * 4;

    VkDeviceMemory stagingBufferMemory;
    VkBuffer stagingBuffer = vktools::create_buffer(
                                                    imageSize, 
                                                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                                                    &stagingBufferMemory, 
                                                    logicalDevice, 
                                                    physicalDevice);

    if(!stagingBuffer)
        return false;

    BufferMemoryDeleter guard = { stagingBufferMemory, stagingBuffer, logicalDevice };

    if (void* data; vkMapMemory(logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data) == VK_SUCCESS)
    {
        memcpy(data, stbImage.pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(logicalDevice, stagingBufferMemory);
    }
    else return false;

    const VkExtent2D extent = { static_cast<uint32_t>(stbImage.width), static_cast<uint32_t>(stbImage.height) };

    if(image = vktools::create_image_2D(
                                        extent, 
                                        VK_FORMAT_R8G8B8A8_SRGB, 
                                        VK_IMAGE_TILING_OPTIMAL, 
                                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                        &imageMemory); !image)
        return false;
        
    if ( ! vktools::transition_image_layout(
                                            image, 
                                            VK_FORMAT_R8G8B8A8_SRGB, 
                                            VK_IMAGE_LAYOUT_UNDEFINED, 
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                                            pool))
        return false;

    if ( ! vktools::copy_buffer_to_image(
                                         stagingBuffer, 
                                         image, 
                                         static_cast<uint32_t>(stbImage.width), 
                                         static_cast<uint32_t>(stbImage.height), 
                                         pool) )
        return false;

    if ( ! vktools::transition_image_layout(
                                            image, 
                                            VK_FORMAT_R8G8B8A8_SRGB, 
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
                                            pool) )
        return false;

    if(imageView = vktools::create_image_view_2D(image, 
                                                 VK_FORMAT_R8G8B8A8_SRGB, 
                                                 VK_IMAGE_ASPECT_COLOR_BIT); !imageView)
        return false;
    
    if ( ! create_sampler(this, physicalDevice, logicalDevice) )
        return false;

    return true;
}


void Texture2D::destroy() noexcept
{
    const auto logicalDevice = vkContext->getLogicalDevice();

    vkDestroySampler(logicalDevice, sampler, VK_NULL_HANDLE);
    vkDestroyImageView(logicalDevice, imageView, VK_NULL_HANDLE);
    vkDestroyImage(logicalDevice, image, VK_NULL_HANDLE);
    vkFreeMemory(logicalDevice, imageMemory, VK_NULL_HANDLE);
}

namespace
{
    bool create_sampler(Texture2D* texture, VkPhysicalDevice gpu, VkDevice device) noexcept
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(gpu, &properties);

        const VkSamplerCreateInfo samplerInfo = 
        {
            .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext                   = VK_NULL_HANDLE,
            .flags                   = 0,
            .magFilter               = VK_FILTER_LINEAR,
            .minFilter               = VK_FILTER_LINEAR,
            .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias              = 0.f,
            .anisotropyEnable        = VK_TRUE,
            .maxAnisotropy           = properties.limits.maxSamplerAnisotropy,
            .compareEnable           = VK_FALSE,
            .compareOp               = VK_COMPARE_OP_ALWAYS,
            .minLod                  = 0.f,
            .maxLod                  = 0.f,
            .borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE
        };

        return (vkCreateSampler(device, &samplerInfo, VK_NULL_HANDLE, &texture->sampler) == VK_SUCCESS);
    }
}
