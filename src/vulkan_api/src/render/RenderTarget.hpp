#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include <vulkan/vulkan.h>

class RenderTarget
{
public:
    RenderTarget() noexcept;

    bool begin(VkCommandBuffer cmd, uint32_t imageIndex) noexcept;
    void draw(class Drawable& object)                    noexcept;
    bool end(VkCommandBuffer cmd, uint32_t imageIndex)   noexcept;

    uint32_t getIndex() const noexcept;

private:
    VkCommandBuffer m_commandBuffer;
    uint32_t m_currentFrame;
};

#endif // !RENDER_TARGET_HPP