#include <cstring>
#include <array>

#include <cglm/struct/mat4.h>

#include "utils/Tools.hpp"
#include "context/Context.hpp"
#include "view/swapchain/Swapchain.hpp"
#include "view/View.hpp"
#include "pipeline/state/PipelineState.hpp"
#include "pipeline/GraphicsPipeline.hpp"


bool GraphicsPipeline::create(const PipelineState& state) noexcept
{
    destroy(); // for recreate case

    const auto physicalDevice = vkContext->getPhysicalDevice(); 
    const auto logicalDevice = vkContext->getLogicalDevice();

    const VkFormat colorFormat = vkView->getSwapchain()->getColorAttachment(0).format;
    const VkFormat depthFormat = vkView->getSwapchain()->getDepthAttachment().format;

    const VkPipelineVertexInputStateCreateInfo vertexInput = state.vertexInputState.getInfo();

    const VkPipelineRenderingCreateInfoKHR pipelineRenderingInfo =
    {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .pNext                   = VK_NULL_HANDLE,
        .viewMask                = 0,
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &colorFormat,
        .depthAttachmentFormat   = depthFormat,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED
    };

    const VkPipelineColorBlendStateCreateInfo colorBlending =
    {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = VK_NULL_HANDLE,
        .flags           = 0,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = &state.colorBlending
    };

    const std::array<VkDynamicState, 2> dynamicStates = 
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    const VkPipelineDynamicStateCreateInfo dynamicStateInfo = 
    {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = VK_NULL_HANDLE,
        .flags             = 0,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates    = dynamicStates.data()
    };

    if (vkCreateDescriptorSetLayout(logicalDevice, &state.layoutInfo, VK_NULL_HANDLE, &descriptorSetLayout) != VK_SUCCESS)
        return false;

    const VkPipelineLayoutCreateInfo pipelineLayoutInfo = 
    {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = VK_NULL_HANDLE,
        .flags                  = 0,
        .setLayoutCount         = 1,
        .pSetLayouts            = &descriptorSetLayout,
        .pushConstantRangeCount = static_cast<uint32_t>(state.constantRanges.size()),
        .pPushConstantRanges    = state.constantRanges.data()
    };

    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, VK_NULL_HANDLE, &layout) != VK_SUCCESS)
        return false;

    const VkPipelineDepthStencilStateCreateInfo depthStencilInfo = 
    {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = VK_NULL_HANDLE,
        .flags                 = 0,
        .depthTestEnable       = VK_TRUE,
        .depthWriteEnable      = VK_TRUE,
        .depthCompareOp        = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = {},
        .back                  = {},
        .minDepthBounds        = 0.f,
        .maxDepthBounds        = 1.f
    };

    const VkGraphicsPipelineCreateInfo pipelineInfo = 
    {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &pipelineRenderingInfo,
        .flags               = 0,
        .stageCount          = static_cast<uint32_t>(state.shaderInfo.size()),
        .pStages             = state.shaderInfo.data(),
        .pVertexInputState   = &vertexInput,
        .pInputAssemblyState = &state.inputAssembly,
        .pTessellationState  = VK_NULL_HANDLE,
        .pViewportState      = &state.viewportState,
        .pRasterizationState = &state.rasterizer,
        .pMultisampleState   = &state.multisampling,
        .pDepthStencilState  = &depthStencilInfo,
        .pColorBlendState    = &colorBlending,
        .pDynamicState       = &dynamicStateInfo,
        .layout              = layout,
        .renderPass          = VK_NULL_HANDLE,
        .subpass             = 0,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = 0
    };

    return (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, VK_NULL_HANDLE, &handle) == VK_SUCCESS);
}


void GraphicsPipeline::destroy() noexcept
{
    const auto logicalDevice = vkContext->getLogicalDevice();

    if (handle)
        vkDestroyPipeline(logicalDevice, handle, VK_NULL_HANDLE);

    if (layout)
        vkDestroyPipelineLayout(logicalDevice, layout, VK_NULL_HANDLE);

    if (descriptorSetLayout)
        vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, VK_NULL_HANDLE);

    handle              = VK_NULL_HANDLE;
    layout              = VK_NULL_HANDLE;
    descriptorSetLayout = VK_NULL_HANDLE; 
}