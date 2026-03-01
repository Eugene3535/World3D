#ifndef GRAPHICS_PIPELINE_HPP
#define GRAPHICS_PIPELINE_HPP

#include <vector>
#include <span>

#include "presentation/MainView.hpp"
#include "pipeline/stages/shader/Shader.hpp"
#include "pipeline/stages/shader/VertexInputState.hpp"
#include "pipeline/stages/uniform/DescriptorSetLayout.hpp"


struct GraphicsPipeline
{
    struct State
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
        DescriptorSetLayout                          layoutInfo;
    };

    bool create(const State& state, const MainView& view) noexcept;
    void destroy(VkDevice device) noexcept;

    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout      layout;
    VkPipeline            handle;
};

#endif // !GRAPHICS_PIPELINE_HPP