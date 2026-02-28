#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "pipeline/descriptors/DescriptorPool.hpp"
#include "pipeline/GraphicsPipeline.hpp"
#include "command_pool/CommandBufferPool.hpp"
#include "sync/SyncManager.hpp"
#include "texture/texture2D.hpp"
#include "buffers/BufferHolder.hpp"
#include "render/Renderer.hpp"
#include "camera/camera.hpp"


struct VulkanApp
{
    bool create(const char* title, int width, int height) noexcept;
    int  run() noexcept;
    void destroy() noexcept;

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

    bool framebufferResized;
    int32_t width;
    int32_t height;

    Camera camera;
    mat4s modelViewProjectionMatrix;

    GLFWwindow* window;
};

#endif // !APPLICATION_H