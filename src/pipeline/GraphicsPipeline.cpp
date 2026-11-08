#include <array>
#include <cstring>

#include <cglm/struct/mat4.h>

#include "context/Context.hpp"
#include "pipeline/GraphicsPipeline.hpp"
#include "pipeline/state/PipelineState.hpp"
#include "utils/Tools.hpp"
#include "view/View.hpp"
#include "view/swapchain/Swapchain.hpp"


bool GraphicsPipeline::create(const PipelineState& state) noexcept
{
    destroy(); // for recreate case

    const auto physicalDevice = vkContext->get<VkPhysicalDevice>();
    const auto logicalDevice  = vkContext->get<VkDevice>();

    const VkFormat colorFormat = vkView->getSwapchain()->getColorAttachment(0).format;
    const VkFormat depthFormat = vkView->getSwapchain()->getDepthAttachment().format;

    const VkPipelineVertexInputStateCreateInfo vertexInput = state.vertexInputState.getInfo();

    const VkPipelineRenderingCreateInfoKHR pipelineRenderingInfo =
        {.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
         .pNext                   = nullptr,
         .viewMask                = 0,
         .colorAttachmentCount    = 1,
         .pColorAttachmentFormats = &colorFormat,
         .depthAttachmentFormat   = depthFormat,
         .stencilAttachmentFormat = VK_FORMAT_UNDEFINED};

    const VkPipelineColorBlendStateCreateInfo colorBlending =
        {.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
         .pNext           = nullptr,
         .flags           = 0,
         .logicOpEnable   = VK_FALSE,
         .logicOp         = VK_LOGIC_OP_COPY,
         .attachmentCount = 1,
         .pAttachments    = &state.colorBlending};

    const std::array<VkDynamicState, 2> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    const VkPipelineDynamicStateCreateInfo dynamicStateInfo =
        {.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
         .pNext             = nullptr,
         .flags             = 0,
         .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
         .pDynamicStates    = dynamicStates.data()};

    if (vkCreateDescriptorSetLayout(logicalDevice, &state.layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        return false;

    const VkPushConstantRange pushConstantRange = {.stageFlags = VK_SHADER_STAGE_VERTEX_BIT, .offset = 0, .size = sizeof(mat4s)};

    const VkPipelineLayoutCreateInfo pipelineLayoutInfo =
        {.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
         .pNext                  = nullptr,
         .flags                  = 0,
         .setLayoutCount         = 1,
         .pSetLayouts            = &descriptorSetLayout,
         .pushConstantRangeCount = 1,
         .pPushConstantRanges    = &pushConstantRange};

    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS)
        return false;

    const VkPipelineDepthStencilStateCreateInfo depthStencilInfo =
        {.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
         .pNext                 = nullptr,
         .flags                 = 0,
         .depthTestEnable       = VK_TRUE,
         .depthWriteEnable      = VK_TRUE,
         .depthCompareOp        = VK_COMPARE_OP_LESS,
         .depthBoundsTestEnable = VK_FALSE,
         .stencilTestEnable     = VK_FALSE,
         .front                 = {},
         .back                  = {},
         .minDepthBounds        = 0.f,
         .maxDepthBounds        = 1.f};

    const VkGraphicsPipelineCreateInfo pipelineInfo =
        {.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
         .pNext               = &pipelineRenderingInfo,
         .flags               = 0,
         .stageCount          = static_cast<uint32_t>(state.shaderInfo.size()),
         .pStages             = state.shaderInfo.data(),
         .pVertexInputState   = &vertexInput,
         .pInputAssemblyState = &state.inputAssembly,
         .pTessellationState  = nullptr,
         .pViewportState      = &state.viewportState,
         .pRasterizationState = &state.rasterizer,
         .pMultisampleState   = &state.multisampling,
         .pDepthStencilState  = &depthStencilInfo,
         .pColorBlendState    = &colorBlending,
         .pDynamicState       = &dynamicStateInfo,
         .layout              = layout,
         .renderPass          = nullptr,
         .subpass             = 0,
         .basePipelineHandle  = nullptr,
         .basePipelineIndex   = 0};

    return (vkCreateGraphicsPipelines(logicalDevice, nullptr, 1, &pipelineInfo, nullptr, &handle) == VK_SUCCESS);
}


void GraphicsPipeline::destroy() noexcept
{
    const auto logicalDevice = vkContext->get<VkDevice>();

    if (handle)
        vkDestroyPipeline(logicalDevice, handle, nullptr);

    if (layout)
        vkDestroyPipelineLayout(logicalDevice, layout, nullptr);

    if (descriptorSetLayout)
        vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);

    handle              = nullptr;
    layout              = nullptr;
    descriptorSetLayout = nullptr;
}