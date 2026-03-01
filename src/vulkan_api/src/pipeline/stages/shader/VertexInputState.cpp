#include "pipeline/stages/shader/VertexInputState.hpp"


static uint32_t shader_attribute_type_to_component_count(const VertexInputState::AttributeType type) noexcept;
static size_t   shader_attribute_type_sizeof(const VertexInputState::AttributeType type)             noexcept;
static VkFormat shader_attribute_type_to_vk_format(const VertexInputState::AttributeType type)       noexcept;



void VertexInputState::create(std::span<const VertexInputState::AttributeType> attributes) noexcept
{
    attributeDescriptions.resize(attributes.size());
    VkVertexInputAttributeDescription* descriptions = attributeDescriptions.data();
    uint32_t offset = 0;

    for (uint32_t i = 0; i < attributes.size(); ++i)
    {
        descriptions[i].location = i;
        descriptions[i].binding  = 0;
        descriptions[i].format   = shader_attribute_type_to_vk_format(attributes[i]);
        descriptions[i].offset   = offset;

        offset += shader_attribute_type_sizeof(attributes[i]);
    }

    bindingDescription.binding   = 0;
    bindingDescription.stride    = offset;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}


VkPipelineVertexInputStateCreateInfo VertexInputState::getInfo() const noexcept
{
    const VkPipelineVertexInputStateCreateInfo info = 
    {
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = VK_NULL_HANDLE,
        .flags                           = 0,
        .vertexBindingDescriptionCount   = 1,
        .pVertexBindingDescriptions      = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions    = attributeDescriptions.data()
    };

    return info;
}



uint32_t shader_attribute_type_to_component_count(const VertexInputState::AttributeType type) noexcept
{
    switch (type)
    {
        case VertexInputState::Float1:
        case VertexInputState::Int1:
            return 1;

        case VertexInputState::Float2:
        case VertexInputState::Int2:
            return 2;

        case VertexInputState::Float3:
        case VertexInputState::Int3:
            return 3;

        case VertexInputState::Float4:
        case VertexInputState::Int4:
            return 4;
    }

    return 0;
}


size_t shader_attribute_type_sizeof(const VertexInputState::AttributeType type) noexcept
{
    switch (type)
    {
        case VertexInputState::Float1:
        case VertexInputState::Float2:
        case VertexInputState::Float3:
        case VertexInputState::Float4:
            return sizeof(float) * shader_attribute_type_to_component_count(type);

        case VertexInputState::Int1:
        case VertexInputState::Int2:
        case VertexInputState::Int3:
        case VertexInputState::Int4:
            return sizeof(int32_t) * shader_attribute_type_to_component_count(type);
    }

    return 0;
}


VkFormat shader_attribute_type_to_vk_format(const VertexInputState::AttributeType type) noexcept
{
    switch (type)
    {
        case VertexInputState::Float1: return VK_FORMAT_R32_SFLOAT;
        case VertexInputState::Float2: return VK_FORMAT_R32G32_SFLOAT;
        case VertexInputState::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
        case VertexInputState::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;

        case VertexInputState::Int1: return VK_FORMAT_R32_SINT;
        case VertexInputState::Int2: return VK_FORMAT_R32G32_SINT;
        case VertexInputState::Int3: return VK_FORMAT_R32G32B32_SINT;
        case VertexInputState::Int4: return VK_FORMAT_R32G32B32A32_SINT;
    }

    return VK_FORMAT_UNDEFINED;
}