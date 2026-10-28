#pragma once

#include <filesystem>

#include <cglm/struct/ivec2.h>
#include <vulkan/vulkan.h>
struct Texture2D
{
    bool loadFromFile(const std::filesystem::path& filepath, VkCommandPool pool) noexcept;
    void destroy() noexcept;

    VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    VkImage        image       = VK_NULL_HANDLE;
    VkImageView    imageView   = VK_NULL_HANDLE;
    VkSampler      sampler     = VK_NULL_HANDLE;

    ivec2s m_size;
};