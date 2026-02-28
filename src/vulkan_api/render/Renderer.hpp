#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vulkan/vulkan.h>

struct Renderer
{
    bool begin(VkCommandBuffer cmd, const struct MainView* view, uint32_t imageIndex) noexcept;
    bool end(VkCommandBuffer cmd, const struct MainView* view, uint32_t imageIndex) noexcept;

    VkClearValue clearColor = { 0.f, 0.f, 0.f, 1.f };
};

#endif // !RENDERER_HPP