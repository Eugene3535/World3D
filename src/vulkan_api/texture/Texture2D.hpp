#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include <vulkan/vulkan.h>

#include "vulkan_api/Export.hpp"

struct VK_API Texture2D
{
    bool loadFromFile(const char* filepath, const struct VulkanContext* context, VkCommandPool pool) noexcept;
    void destroy(VkDevice device) noexcept;

    VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    VkImage        image       = VK_NULL_HANDLE;
    VkImageView    imageView   = VK_NULL_HANDLE;
    VkSampler      sampler     = VK_NULL_HANDLE;
};

#endif // !TEXTURE2D_HPP