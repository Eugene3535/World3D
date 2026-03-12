#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "pipeline/descriptors/DescriptorPool.hpp"
#include "pipeline/GraphicsPipeline.hpp"
#include "command_pool/CommandBufferPool.hpp"
#include "sync/SyncManager.hpp"
#include "texture/Texture2D.hpp"
#include "buffers/BufferHolder.hpp"
#include "render/Renderer.hpp"
#include "camera/Camera.hpp"


struct Engine
{
    bool init(uint64_t windowHandle) noexcept;
    void drawFrame() noexcept;
    void destroy() noexcept;
    void resize(int width, int height) noexcept;

    VulkanContext    context;
    MainView         view;
    GraphicsPipeline pipeline;

    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptorSets;
    DescriptorPool descriptorPool;

    CommandBufferPool commandPool;
    SyncManager sync;

    Texture2D texture;

    BufferHolder bufferHolder;
    Buffer vertices;
    Buffer indices;

    Renderer renderer;

    bool    m_framebufferResized;
    int32_t m_width;
    int32_t m_height;

    Camera camera;
    mat4s modelViewProjectionMatrix;
};

#endif // !ENGINE_HPP