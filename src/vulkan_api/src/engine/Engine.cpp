#include <array>

#include <cglm/struct/affine-pre.h>
#include "spdlog/spdlog.h"
#include <spdlog/sinks/basic_file_sink.h>

#include "view/swapchain/Swapchain.hpp"
#include "engine/Engine.hpp"


// world space positions of our cubes
static const vec3s cubePositions[1] = 
{
    {  0.0f,  0.0f,  0.0f  }
};


Engine::Engine() noexcept
{

}


bool Engine::createContext() noexcept
{
    auto logger = spdlog::basic_logger_mt("logger", "logs/log.txt", true);
    logger->set_level(spdlog::level::debug);
    spdlog::set_default_logger(logger);
    spdlog::info("Start logging messages");

    if (!m_context.create())
        return false;

    return true;
}


bool Engine::createMainView(uint64_t windowHandle) noexcept
{
    if (!m_view.create(windowHandle))
        return false;

    if (!m_sync.create())
		return false;

    return true;
}


bool Engine::createPipeline() noexcept
{
	auto rootScene = std::make_unique<RootScene>(nullptr);

    if (rootScene->create())
    {
        m_rootScene.swap(rootScene);

	    return true;
    }

    return false;
}


void Engine::drawFrame() noexcept
{
    if ( ! (m_width && m_height) )
        return;

	uint32_t frame  = m_sync.currentFrame;
    const auto logicalDevice = vkContext->getLogicalDevice();
    const auto queue = vkContext->getQueue();

    VkResult result = vkWaitForFences(logicalDevice, 1, &m_sync.inFlightFences[frame], VK_TRUE, UINT64_MAX);

	if (result != VK_SUCCESS)
    {
#ifdef DEBUG
        printf("failed to wait for fences!\n");
#endif
		return;
    }

    uint32_t imageIndex;
    result = vkAcquireNextImageKHR(logicalDevice, m_view.getSwapchain()->getHandle(), UINT64_MAX, m_sync.imageAvailableSemaphores[frame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        vkDeviceWaitIdle(logicalDevice);
		m_view.resize();

        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
#ifdef DEBUG
        printf("failed to acquire swap chain image!\n");
#endif
		return;
    }

    result = vkResetFences(logicalDevice, 1, &m_sync.inFlightFences[frame]);

	if (result != VK_SUCCESS)
    {
#ifdef DEBUG
        printf("failed to reset fences!\n");
#endif
		return;
    }

    VkCommandBuffer commandBuffer = m_rootScene->m_commandPool.commandBuffers[frame];
    VkDescriptorSet descriptorSet = m_rootScene->m_descriptorSets[frame];

    if (!m_renderer.begin(commandBuffer, imageIndex))
        return;

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_rootScene->m_pipeline.handle);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_rootScene->m_pipeline.layout, 0, 1, &descriptorSet, 0, VK_NULL_HANDLE);

    mat4s projection = glms_perspective(glm_rad(60.f), m_width / (float)m_height, 0.1f, 100.f);
    mat4s viewMatrix  = camera.getViewMatrix();
    vec3s axis = { 1.0f, 0.3f, 0.5f };

//  update matrices
    mat4s model = glms_translate(glms_mat4_identity(), cubePositions[0]);
    model       = glms_rotate(model, glm_rad(0), axis);
    mat4s modelViewProjection = glms_mat4_mul(glms_mat4_mul(projection, viewMatrix), model);

    void* data;
    vkMapMemory(logicalDevice, m_rootScene->m_uniformBuffers[imageIndex].memory, 0, sizeof(mat4s), 0, &data);
    memcpy(data, &modelViewProjection, sizeof(mat4s));
    vkUnmapMemory(logicalDevice, m_rootScene->m_uniformBuffers[imageIndex].memory);

//  write command buffer
    VkDeviceSize offsets[] = {0};
    VkBuffer vertexBuffers[] = { m_rootScene->m_vertexBuffer.handle };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_rootScene->m_indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, m_rootScene->m_indexBuffer.size, 1, 0, 0, 0);

    if (!m_renderer.end(commandBuffer, imageIndex))
        return;

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
    const VkSubmitInfo submitInfo = 
	{
		.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext                = VK_NULL_HANDLE,
		.waitSemaphoreCount   = 1,
		.pWaitSemaphores      = m_sync.imageAvailableSemaphores.data() + frame,
		.pWaitDstStageMask    = waitStages,
		.commandBufferCount   = 1,
		.pCommandBuffers      = &m_rootScene->m_commandPool.commandBuffers[frame],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores    = &m_sync.renderFinishedSemaphores[frame]
	};

	result = vkQueueSubmit(queue, 1, &submitInfo, m_sync.inFlightFences[frame]);

    if (result != VK_SUCCESS)
    {
#ifdef DEBUG
        printf("failed to submit draw command buffer!\n");
#endif
		return;
    }

    const VkPresentInfoKHR presentInfo = 
	{
		.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext              = VK_NULL_HANDLE,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores    = &m_sync.renderFinishedSemaphores[frame],
		.swapchainCount     = 1,
		.pSwapchains        = &vkView->getSwapchain()->getHandle(),
		.pImageIndices      = &imageIndex,
		.pResults           = VK_NULL_HANDLE
	};

    result = vkQueuePresentKHR(queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
    {
        m_framebufferResized = false;
        vkDeviceWaitIdle(logicalDevice);
		m_view.resize();
    }
    else if (result != VK_SUCCESS)
    {
#ifdef DEBUG
        printf("failed to present swap chain image!!\n");
#endif
		return;
    }

    m_sync.currentFrame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;

	vkDeviceWaitIdle(logicalDevice);
}


void Engine::destroy() noexcept
{
	const auto logicalDevice = vkContext->getLogicalDevice();

	m_rootScene.reset();
	m_sync.destroy();
	m_view.destroy();
}


void Engine::resize(int width, int height) noexcept
{
	m_width = width;
	m_height = height;
	m_framebufferResized = true;
}