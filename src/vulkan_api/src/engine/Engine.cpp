#include <array>

#include <cglm/struct/affine-pre.h>
#include "spdlog/spdlog.h"
#include <spdlog/sinks/basic_file_sink.h>

#include "pipeline/descriptors/DescriptorSetLayout.hpp"
#include "pipeline/state/PipelineState.hpp"
#include "engine/Engine.hpp"


// world space positions of our cubes
static const vec3s cubePositions[10] = 
{
    {  0.0f,  0.0f,  0.0f  },
    {  2.0f,  5.0f, -15.0f },
    { -1.5f, -2.2f, -2.5f  },
    { -3.8f, -2.0f, -12.3f },
    {  2.4f, -0.4f, -3.5f  },
    { -1.7f,  3.0f, -7.5f  },
    {  1.3f, -2.0f, -2.5f  },
    {  1.5f,  2.0f, -2.5f  },
    {  1.5f,  0.2f, -1.5f  },
    { -1.3f,  1.0f, -1.5f  }
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
	VkDevice device = m_context.getLogicalDevice();

	{// Pipeline
		std::array<Shader, 2> shaders = { Shader(device), Shader(device) };

		if (!shaders[0].loadFromFile("res/shaders/vertex_shader.spv", VK_SHADER_STAGE_VERTEX_BIT))
			return false;

		if (!shaders[1].loadFromFile("res/shaders/fragment_shader.spv", VK_SHADER_STAGE_FRAGMENT_BIT))
			return false;

        std::array<const VertexInputState::AttributeType, 2> attributes =
        {
            VertexInputState::Float3,
            VertexInputState::Float2
        };

        DescriptorSetLayout uniformDescriptors;
        uniformDescriptors.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        uniformDescriptors.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

        PipelineState pipelineState;
        pipelineState.setupShaderStages(shaders, attributes);
        pipelineState.setupInputAssembler(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineState.setupViewport();
        pipelineState.setupRasterization(VK_POLYGON_MODE_FILL);
        pipelineState.setupMultisampling();
        pipelineState.setupColorBlending(VK_FALSE);
        pipelineState.layoutInfo = uniformDescriptors.getInfo();
            
		if (!m_pipeline.create(pipelineState))
			return false;
	}

	{// Descriptors
		const std::array<VkDescriptorPoolSize, 2> poolSizes = 
		{
			VkDescriptorPoolSize
			{
				.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = MAX_FRAMES_IN_FLIGHT
			},
            VkDescriptorPoolSize
			{
				.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = MAX_FRAMES_IN_FLIGHT
			}
		};

		if (!m_descriptorPool.create(poolSizes))
			return false;

		VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT] = 
		{ 
			m_pipeline.descriptorSetLayout, 
			m_pipeline.descriptorSetLayout 
		};

		if (!m_descriptorPool.allocateDescriptorSets(m_descriptorSets, layouts))
			return false;	
	}

	if (!m_commandPool.create())
        return false;

    {
        m_uniformBuffers.resize(vkView->getImageCount());
        std::array<mat4s, 1> identity = { glms_mat4_identity() };

        for (size_t i = 0; i < m_uniformBuffers.size(); ++i) 
            m_uniformBuffers[i] = m_bufferHolder.allocate<mat4s>(identity,
                                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                                                                 m_commandPool.handle);
    }

	{
        if(!m_texture.loadFromFile("res/textures/container.jpg", m_commandPool.handle))
            return false;

        const std::array<VkDescriptorBufferInfo, 2> bufferInfos = 
        {
            VkDescriptorBufferInfo
            {
                .buffer = m_uniformBuffers[0].handle,
                .offset = 0,
                .range = sizeof(mat4s)
            },
            VkDescriptorBufferInfo
            {
                .buffer = m_uniformBuffers[1].handle,
                .offset = 0,
                .range = sizeof(mat4s)
            }
        };
                
        const VkDescriptorImageInfo imageInfo = 
        {
            .sampler     = m_texture.sampler,
            .imageView   = m_texture.imageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

		m_descriptorPool.writeBufferInfo(bufferInfos.data(), m_descriptorSets[0], 0);
		m_descriptorPool.writeBufferInfo(bufferInfos.data() + 1, m_descriptorSets[1], 0);
		m_descriptorPool.writeCombinedImageSampler(&imageInfo, m_descriptorSets[0], 1);
		m_descriptorPool.writeCombinedImageSampler(&imageInfo, m_descriptorSets[1], 1);
    }

	{
	    constexpr std::array<float, 120> vertices = 
        {
            -0.5f, -0.5f, 0.5f, 0.f, 0.f,
             0.5f, -0.5f, 0.5f, 1.f, 0.f,
             0.5f,  0.5f, 0.5f, 1.f, 1.f,
            -0.5f,  0.5f, 0.5f, 0.f, 1.f,

            -0.5f, -0.5f, -0.5f, 0.f, 0.f,
            -0.5f, -0.5f,  0.5f, 1.f, 0.f,
            -0.5f,  0.5f,  0.5f, 1.f, 1.f,
            -0.5f,  0.5f, -0.5f, 0.f, 1.f,

             0.5f, -0.5f,  0.5f, 0.f, 0.f,
             0.5f, -0.5f, -0.5f, 1.f, 0.f,
             0.5f,  0.5f, -0.5f, 1.f, 1.f,
             0.5f,  0.5f,  0.5f, 0.f, 1.f,

            -0.5f, -0.5f, -0.5f, 0.f, 0.f,
             0.5f, -0.5f, -0.5f, 1.f, 0.f,
             0.5f,  0.5f, -0.5f, 1.f, 1.f,
            -0.5f,  0.5f, -0.5f, 0.f, 1.f,

            -0.5f, 0.5f,  0.5f, 0.f, 0.f,
             0.5f, 0.5f,  0.5f, 1.f, 0.f,
             0.5f, 0.5f, -0.5f, 1.f, 1.f,
            -0.5f, 0.5f, -0.5f, 0.f, 1.f,

            -0.5f, -0.5f, -0.5f, 0.f, 0.f,
             0.5f, -0.5f, -0.5f, 1.f, 0.f,
             0.5f, -0.5f,  0.5f, 1.f, 1.f,
            -0.5f, -0.5f,  0.5f, 0.f, 1.f
        };

		constexpr std::array<uint32_t, 36> indices =
        {
            0,  1,  2,  2,  3,  0,   // front
            4,  5,  6,  6,  7,  4,   // left
            8,  9,  10, 10, 11, 8,   // right
            12, 13, 14, 14, 15, 12,  // back
            16, 17, 18, 18, 19, 16,  // top
            20, 21, 22, 22, 23, 20   // bottom
        };

		m_vertexBuffer = m_bufferHolder.allocate<float>(vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_commandPool.handle);
		m_indexBuffer = m_bufferHolder.allocate<uint32_t>(indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_commandPool.handle);

		if (!m_vertexBuffer.handle)
			return false;

		if (!m_indexBuffer.handle)
			return false;
	}

	return true;
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
    result = vkAcquireNextImageKHR(logicalDevice, m_view.getSwapchain(), UINT64_MAX, m_sync.imageAvailableSemaphores[frame], VK_NULL_HANDLE, &imageIndex);

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

    VkCommandBuffer commandBuffer = m_commandPool.commandBuffers[frame];
    VkDescriptorSet descriptorSet = m_descriptorSets[frame];

    if (!m_renderer.begin(commandBuffer, imageIndex))
        return;

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.handle);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.layout, 0, 1, &descriptorSet, 0, VK_NULL_HANDLE);

    mat4s projection = glms_perspective(glm_rad(60.f), m_width / (float)m_height, 0.1f, 100.f);
    mat4s viewMatrix  = camera.getViewMatrix();
    vec3s axis = { 1.0f, 0.3f, 0.5f };

    for (uint32_t i = 0; i < 10; ++i)
    {
        const float angle = 20.f * i;

//  update matrices
        mat4s model = glms_translate(glms_mat4_identity(), cubePositions[i]);
        model       = glms_rotate(model, glm_rad(angle), axis);
        mat4s modelViewProjection = glms_mat4_mul(glms_mat4_mul(projection, viewMatrix), model);

        void* data;
        vkMapMemory(logicalDevice, m_uniformBuffers[imageIndex].memory, 0, sizeof(mat4s), 0, &data);
        memcpy(data, &modelViewProjection, sizeof(mat4s));
        vkUnmapMemory(logicalDevice, m_uniformBuffers[imageIndex].memory);

//  write command buffer
        VkDeviceSize offsets[] = {0};
        VkBuffer vertexBuffers[] = { m_vertexBuffer.handle };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);
        // vkCmdPushConstants(commandBuffer, m_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4s), modelViewProjection.raw);
        vkCmdDrawIndexed(commandBuffer, m_indexBuffer.size, 1, 0, 0, 0);
    }

    if(!m_renderer.end(commandBuffer, imageIndex))
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
		.pCommandBuffers      = &m_commandPool.commandBuffers[frame],
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
		.pSwapchains        = &vkView->getSwapchain(),
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

	m_bufferHolder.destroy();
	m_texture.destroy();
	m_sync.destroy();
	m_commandPool.destroy();
	m_descriptorPool.destroy();
	m_pipeline.destroy();
	m_view.destroy();
	m_context.destroy();
}


void Engine::resize(int width, int height) noexcept
{
	m_width = width;
	m_height = height;
	m_framebufferResized = true;
}