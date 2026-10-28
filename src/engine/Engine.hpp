#pragma once

#include "buffers/BufferHolder.hpp"
#include "camera/Camera.hpp"
#include "command_pool/CommandBufferPool.hpp"
#include "files/FileProvider.hpp"
#include "pipeline/GraphicsPipeline.hpp"
#include "pipeline/descriptors/DescriptorPool.hpp"
#include "render/Renderer.hpp"
#include "sync/SyncManager.hpp"
#include "texture/Texture2D.hpp"
#include "view/View.hpp"


class Engine
{
public:
    Engine(Camera& camera) noexcept;

    bool createContext() noexcept;
    bool createView(uint64_t windowHandle) noexcept;

    bool createPipeline() noexcept;
    void drawFrame() noexcept;
    void destroy() noexcept;
    void resize(int width, int height) noexcept;

    VulkanContext    m_context;
    View             m_view;
    GraphicsPipeline m_pipeline;

    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> m_descriptorSets;
    DescriptorPool                                    m_descriptorPool;

    CommandBufferPool m_commandPool;
    SyncManager       m_sync;

    Texture2D m_texture;

    std::vector<Buffer> m_uniformBuffers;

    BufferHolder m_bufferHolder;
    Buffer       m_vertexBuffer;
    Buffer       m_indexBuffer;

    Renderer m_renderer;

    bool    m_framebufferResized;
    int32_t m_width;
    int32_t m_height;

    Camera& m_camera;
};