#pragma once

#include <filesystem>

#include <cglm/struct/ivec2.h>
#include <vulkan/vulkan.h>
struct Texture2D
{
    bool loadFromFile(const std::filesystem::path& filepath, VkCommandPool pool) noexcept;
    void destroy() noexcept;

    VkDeviceMemory imageMemory = nullptr;
    VkImage        image       = nullptr;
    VkImageView    imageView   = nullptr;
    VkSampler      sampler     = nullptr;

    ivec2s m_size;
};