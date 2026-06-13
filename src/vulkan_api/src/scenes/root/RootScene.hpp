#ifndef ROOT_SCENE_HPP
#define ROOT_SCENE_HPP

#include "pipeline/descriptors/DescriptorPool.hpp"
#include "pipeline/GraphicsPipeline.hpp"
#include "command_pool/CommandBufferPool.hpp"
#include "texture/Texture2D.hpp"
#include "buffers/BufferHolder.hpp"
#include "scenes/Scene.hpp"


class RootScene: public Scene
{
public:
    RootScene(const Scene* parent) noexcept;
    ~RootScene();

    bool create() noexcept override;

// private:
    GraphicsPipeline m_pipeline;

    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> m_descriptorSets;
    DescriptorPool m_descriptorPool;

    CommandBufferPool m_commandPool;

    Texture2D m_texture;

    std::vector<Buffer> m_uniformBuffers;

    BufferHolder m_bufferHolder;
    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;
};

#endif // !ROOT_SCENE_HPP