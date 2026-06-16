#include "render/RenderTarget.hpp"
#include "render/Drawable.hpp"


Drawable::~Drawable() = default;


void Drawable::draw(RenderTarget& target, VkCommandBuffer cmd) const noexcept
{
    // do nothing
}