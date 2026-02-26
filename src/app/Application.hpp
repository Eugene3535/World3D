#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "vulkan_api/pipeline/descriptors/DescriptorPool.hpp"
#include "vulkan_api/pipeline/GraphicsPipeline.hpp"
#include "vulkan_api/command_pool/CommandBufferPool.hpp"
#include "vulkan_api/sync/SyncManager.hpp"
#include "vulkan_api/texture/texture2D.hpp"
#include "vulkan_api/buffers/BufferHolder.hpp"
#include "vulkan_api/render/Renderer.hpp"
#include "app/camera/camera.hpp"


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