#pragma once


struct GraphicsPipeline
{
    bool create(const struct PipelineState& state) noexcept;
    void destroy() noexcept;

    VkDescriptorSetLayout descriptorSetLayout = nullptr;
    VkPipelineLayout      layout              = nullptr;
    VkPipeline            handle              = nullptr;
};
