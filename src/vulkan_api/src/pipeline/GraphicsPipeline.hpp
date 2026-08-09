#pragma once


struct GraphicsPipeline
{
    bool create(const struct PipelineState& state) noexcept;
    void destroy() noexcept;

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout      layout              = VK_NULL_HANDLE;
    VkPipeline            handle              = VK_NULL_HANDLE;
};
