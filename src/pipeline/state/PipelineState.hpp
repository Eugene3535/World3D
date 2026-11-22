#pragma once

#include <span>
#include <vector>

#include "pipeline/stages/shader/Shader.hpp"
#include "pipeline/stages/shader/VertexInputState.hpp"

struct PipelineState
{
    void addDescriptor(VkDescriptorType type, VkShaderStageFlagBits shaderStage) noexcept;
    void setupShaderStages(std::span<const Shader>                          shaders,
                           std::span<const VertexInputState::AttributeType> attributes) noexcept;

    void setupInputAssembler(const VkPrimitiveTopology primitive) noexcept;
    void setupViewport() noexcept;
    void setupRasterization(VkPolygonMode mode) noexcept;
    void setupMultisampling() noexcept;
    void setupColorBlending(VkBool32 enabled) noexcept;

    std::vector<VkPipelineShaderStageCreateInfo> shaderInfo;
    std::vector<VkPushConstantRange>             constantRanges;
    VertexInputState                             vertexInputState;
    VkPipelineInputAssemblyStateCreateInfo       inputAssembly;
    VkPipelineViewportStateCreateInfo            viewportState;
    VkPipelineRasterizationStateCreateInfo       rasterizer;
    VkPipelineMultisampleStateCreateInfo         multisampling;
    VkPipelineColorBlendAttachmentState          colorBlending;
    VkDescriptorSetLayoutCreateInfo              layoutInfo;

private:
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;
};