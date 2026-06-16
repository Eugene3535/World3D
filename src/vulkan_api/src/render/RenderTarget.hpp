#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include <vulkan/vulkan.h>

class RenderTarget
{
public:
    bool begin(VkCommandBuffer cmd, uint32_t imageIndex) noexcept;
    void draw(class Drawable& object)                    noexcept;
    bool end(VkCommandBuffer cmd, uint32_t imageIndex)   noexcept;

private:
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
};

#endif // !RENDER_TARGET_HPP