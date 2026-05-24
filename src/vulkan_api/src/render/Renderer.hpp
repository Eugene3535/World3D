#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vulkan/vulkan.h>

struct Renderer
{
    bool begin(VkCommandBuffer cmd, const class Swapchain* swapchain, uint32_t imageIndex) noexcept;
    bool end(VkCommandBuffer cmd, const class Swapchain* swapchain, uint32_t imageIndex) noexcept;

    VkClearValue clearColor = { 0.f, 0.f, 0.f, 1.f };
};

#endif // !RENDERER_HPP