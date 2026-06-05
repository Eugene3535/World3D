#ifndef PIPELINE_STATE_HPP
#define PIPELINE_STATE_HPP

#include <vector>
#include <span>

#include "pipeline/stages/shader/Shader.hpp"
#include "pipeline/stages/shader/VertexInputState.hpp"

struct PipelineState
{
    void setupShaderStages(std::span<const Shader> shaders, std::span<const VertexInputState::AttributeType> attributes) noexcept;
    void setupInputAssembler(const VkPrimitiveTopology primitive)                                                        noexcept;
    void setupViewport()                                                                                                 noexcept;
    void setupRasterization(VkPolygonMode mode)                                                                          noexcept;
    void setupMultisampling()                                                                                            noexcept;
    void setupColorBlending(VkBool32 enabled)                                                                            noexcept;

    std::vector<VkPipelineShaderStageCreateInfo> shaderInfo;
    VertexInputState                             vertexInputState;
    VkPipelineInputAssemblyStateCreateInfo       inputAssembly;
    VkPipelineViewportStateCreateInfo            viewportState;
    VkPipelineRasterizationStateCreateInfo       rasterizer;
    VkPipelineMultisampleStateCreateInfo         multisampling;
    VkPipelineColorBlendAttachmentState          colorBlending;
    VkDescriptorSetLayoutCreateInfo              layoutInfo;
};

#endif // !PIPELINE_STATE_HPP