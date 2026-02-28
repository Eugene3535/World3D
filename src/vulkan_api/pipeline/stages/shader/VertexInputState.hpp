#ifndef VERTEX_INPUT_STATE_HPP
#define VERTEX_INPUT_STATE_HPP

#include <vector>
#include <span>

#include <vulkan/vulkan.h>

struct VertexInputState
{
    enum AttributeType : uint32_t
    {
        Float1,
        Float2,
        Float3,
        Float4,
        Int1,
        Int2,
        Int3,
        Int4
    };

    void create(std::span<const VertexInputState::AttributeType> attributes) noexcept;
    VkPipelineVertexInputStateCreateInfo getInfo() const noexcept;

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    VkVertexInputBindingDescription bindingDescription;
};

#endif // !VERTEX_INPUT_STATE_HPP