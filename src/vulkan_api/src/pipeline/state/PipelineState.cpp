#include "pipeline/state/PipelineState.hpp"


void PipelineState::setupShaderStages(std::span<const Shader> shaders, std::span<const VertexInputState::AttributeType> attributes) noexcept
{
    for(const auto& shader : shaders)
        shaderInfo.emplace_back(shader.getInfo());

    vertexInputState.create(attributes);
}


void PipelineState::setupInputAssembler(const VkPrimitiveTopology primitive) noexcept
{
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext                  = VK_NULL_HANDLE;
    inputAssembly.flags                  = 0;
    inputAssembly.topology               = primitive;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
}


void PipelineState::setupViewport() noexcept
{
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext         = VK_NULL_HANDLE;
    viewportState.flags         = 0;
    viewportState.viewportCount = 1;
    viewportState.pViewports    = VK_NULL_HANDLE;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = VK_NULL_HANDLE;
}


void PipelineState::setupRasterization(VkPolygonMode mode) noexcept
{
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.pNext                   = VK_NULL_HANDLE;
    rasterizer.flags                   = 0;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = mode;
    rasterizer.cullMode                = VK_CULL_MODE_NONE; // TODO Добавить переключение отсечения граней, например VK_CULL_MODE_FRONT_BIT
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.f;
    rasterizer.depthBiasClamp          = 0.f;
    rasterizer.depthBiasSlopeFactor    = 0.f;
    rasterizer.lineWidth               = 1.f;
}


void PipelineState::setupMultisampling() noexcept
{
    multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.pNext                 = VK_NULL_HANDLE;
    multisampling.flags                 = 0;
    multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable   = VK_FALSE;
    multisampling.minSampleShading      = 1.f;
    multisampling.pSampleMask           = VK_NULL_HANDLE;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable      = VK_FALSE;
}


void PipelineState::setupColorBlending(VkBool32 enabled) noexcept
{
    colorBlending.blendEnable         = enabled;
    colorBlending.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlending.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlending.colorBlendOp        = VK_BLEND_OP_ADD;
    colorBlending.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlending.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlending.alphaBlendOp        = VK_BLEND_OP_ADD;
    colorBlending.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
}