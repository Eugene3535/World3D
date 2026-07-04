#ifndef ROOT_SCENE_HPP
#define ROOT_SCENE_HPP

#include <array>

#include "pipeline/descriptors/DescriptorPool.hpp"
#include "pipeline/GraphicsPipeline.hpp"
#include "texture/Texture2D.hpp"
#include "buffers/BufferHolder.hpp"
#include "scenes/Scene.hpp"
#include "render/Drawable.hpp"


class RootScene: 
    public Scene,
    public Drawable
{
public:
    RootScene(const Scene* parent) noexcept;
    ~RootScene();

    bool create(void* commandPool) noexcept override;

// private:
    GraphicsPipeline m_pipeline;

    DescriptorPool m_descriptorPool;
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> m_descriptorSets;

    Texture2D m_texture;

    std::vector<Buffer> m_uniformBuffers;

    BufferHolder m_bufferHolder;
    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;

private:
    void draw(class RenderTarget& target, VkCommandBuffer cmd) const noexcept override;
};

#endif // !ROOT_SCENE_HPP