#pragma once


class GraphicsPipeline
{
public:
    bool create(const struct PipelineState& state) noexcept;
    void destroy() noexcept;


    VkPipeline            handle              = nullptr;
    VkPipelineLayout      layout              = nullptr;
    VkDescriptorSetLayout descriptorSetLayout = nullptr;
};
