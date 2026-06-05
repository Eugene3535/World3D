#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "view/View.hpp"
#include "pipeline/descriptors/DescriptorPool.hpp"
#include "pipeline/GraphicsPipeline.hpp"
#include "command_pool/CommandBufferPool.hpp"
#include "sync/SyncManager.hpp"
#include "texture/Texture2D.hpp"
#include "buffers/BufferHolder.hpp"
#include "render/Renderer.hpp"
#include "camera/Camera.hpp"


class Engine
{
public:
    Engine() noexcept;

    bool createContext()                       noexcept;
    bool createMainView(uint64_t windowHandle) noexcept;

    bool createPipeline() noexcept;
    void drawFrame() noexcept;
    void destroy() noexcept;
    void resize(int width, int height) noexcept;

    VulkanContext    m_context;
    View             m_view;
    GraphicsPipeline m_pipeline;

    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> m_descriptorSets;
    DescriptorPool m_descriptorPool;

    CommandBufferPool m_commandPool;
    SyncManager m_sync;

    Texture2D m_texture;

    std::vector<Buffer> m_uniformBuffers;

    BufferHolder m_bufferHolder;
    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;

    Renderer m_renderer;

    bool    m_framebufferResized;
    int32_t m_width;
    int32_t m_height;

    Camera camera;
};

#endif // !ENGINE_HPP