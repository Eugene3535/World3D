#pragma once

#include <vulkan/vulkan.h>

struct Renderer
{
    bool begin(VkCommandBuffer cmd, uint32_t imageIndex) noexcept;
    bool end(VkCommandBuffer cmd, uint32_t imageIndex) noexcept;

    VkClearValue clearColor = { 0.f, 0.f, 0.f, 1.f };
};