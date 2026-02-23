#ifdef DEBUG
#include <cstdio>
#endif
#include <array>

#include <cglm/struct/affine-pre.h>

#include "vulkan_api/context/Context.hpp"
#include "app/Application.hpp"


static bool init_vulkan(VulkanApp* app) noexcept;
static void update_matrices(VulkanApp* app, vec3s position, float angle) noexcept;
static void write_command_buffer(VulkanApp* app, VkCommandBuffer cmd, VkDescriptorSet descriptorSet) noexcept;
static void draw_frame(VulkanApp* app) noexcept;


#define FPS_MEASUREMENT

// TODO remove magic numbers
static float lastX = 400;
static float lastY = 300;


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


bool VulkanApp::create(const char* title, int width, int height) noexcept
{
	if(glfwInit() == GLFW_TRUE)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		if(window = glfwCreateWindow(width, height, title, nullptr, nullptr))
		{
			glfwSetWindowUserPointer(window, this);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				if(auto app = static_cast<VulkanApp*>(glfwGetWindowUserPointer(window)))
				{
					app->width = width;
					app->height = height;
					app->framebufferResized = true;
				}
			});

			glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
					glfwSetWindowShouldClose(window, GLFW_TRUE);
			});

			glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xposIn, double yposIn)
			{
				if(auto app = static_cast<VulkanApp*>(glfwGetWindowUserPointer(window)))
				{
					float xpos = (float)xposIn;
					float ypos = (float)yposIn;

					float xoffset = xpos - lastX;
					float yoffset = ypos - lastY; // reversed since y-coordinates go from bottom to top

					lastX = xpos;
					lastY = ypos;

					app->camera.processMouseMovement(xoffset, yoffset);
				}
			});
			
			return init_vulkan(this);
		}
	}

	return false;
}


int VulkanApp::run() noexcept
{
	modelViewProjectionMatrix = glms_mat4_identity();

	float deltaTime = 0.f;
	float lastFrame = 0.f;

#ifdef FPS_MEASUREMENT
    float timer = 0;
    int fps = 0;
#endif
    
	while(!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

#ifdef FPS_MEASUREMENT
        timer += deltaTime;
        ++fps;

        if (timer > 1.f)
        {
            printf("FPS: %i\n", fps);
            timer = 0;
            fps = 0;

        }
#endif

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        	glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.processKeyboard(Camera::FORWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.processKeyboard(Camera::BACKWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.processKeyboard(Camera::LEFT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.processKeyboard(Camera::RIGHT, deltaTime);

		draw_frame(this);

		glfwPollEvents();
	}

	vkDeviceWaitIdle(context.device);

	return 0;
}


void VulkanApp::destroy() noexcept
{
	VkDevice device = context.device;

	bufferHolder.destroy(device);
	texture.destroy(device);
	sync.destroy(device);
	commandPool.destroy(device);
	descriptorPool.destroy(device);
	pipeline.destroy(device);

	view.destroy();
	context.destroy();
	
	glfwDestroyWindow(window);
	glfwTerminate();
}


bool init_vulkan(VulkanApp* app) noexcept
{
    glfwGetFramebufferSize(app->window, &app->width, &app->height);

	if(!app->context.create())
		return false;

	app->view.context = &app->context;
	
	if(!app->view.create(app->window))
		return false;

	VkDevice device = app->context.device;

	{// Pipeline
		std::array<Shader, 2> shaders;

		if(!shaders[0].loadFromFile("res/shaders/vertex_shader.spv", VK_SHADER_STAGE_VERTEX_BIT, device))
			return false;

		if(!shaders[1].loadFromFile("res/shaders/fragment_shader.spv", VK_SHADER_STAGE_FRAGMENT_BIT, device))
			return false;

        std::array<const VertexInputState::AttributeType, 2> attributes =
        {
            VertexInputState::Float3,
            VertexInputState::Float2
        };

        DescriptorSetLayout uniformDescriptors;
        uniformDescriptors.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

        GraphicsPipeline::State pipelineState;
        pipelineState.setupShaderStages(shaders, attributes);
        pipelineState.setupInputAssembler(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineState.setupViewport();
        pipelineState.setupRasterization(VK_POLYGON_MODE_FILL);
        pipelineState.setupMultisampling();
        pipelineState.setupColorBlending(VK_FALSE);
        pipelineState.layoutInfo = uniformDescriptors;

        bool result = app->pipeline.create(pipelineState, app->view);
            
		if(!result)
			return false;
	}

	{// Descriptors
		std::array<VkDescriptorPoolSize, 1> poolSizes = 
		{
			VkDescriptorPoolSize
			{
				.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = MAX_FRAMES_IN_FLIGHT
			}
		};

		if(!app->descriptorPool.create(poolSizes, device))
			return false;

		VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT] = 
		{ 
			app->pipeline.descriptorSetLayout, 
			app->pipeline.descriptorSetLayout 
		};

		if(!app->descriptorPool.allocateDescriptorSets(app->descriptorSets, layouts, device))
			return false;	
	}

	if(!app->commandPool.create(device, app->context.mainQueueFamilyIndex))
        return false;

	if(!app->sync.create(device))
		return false;

	{
        if(!app->texture.loadFromFile("res/textures/container.jpg", &app->context, app->commandPool.handle))
            return false;
                
        const VkDescriptorImageInfo imageInfo = 
        {
            .sampler     = app->texture.sampler,
            .imageView   = app->texture.imageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

		app->descriptorPool.writeCombinedImageSampler(&imageInfo, app->descriptorSets[0], 0, device);
		app->descriptorPool.writeCombinedImageSampler(&imageInfo, app->descriptorSets[1], 0, device);
    }

	{
	    const std::array<float, 120> vertices = 
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

        const std::array<uint32_t, 36> indices = 
        {
            0,  1,  2,  2,  3,  0,   // front
            4,  5,  6,  6,  7,  4,   // left
            8,  9,  10, 10, 11, 8,   // right
            12, 13, 14, 14, 15, 12,  // back
            16, 17, 18, 18, 19, 16,  // top
            20, 21, 22, 22, 23, 20   // bottom
        };

		app->vertices = app->bufferHolder.allocate<float>(vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &app->context, app->commandPool.handle);
		app->indices = app->bufferHolder.allocate<uint32_t>(indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &app->context, app->commandPool.handle);

		if(!app->vertices.handle)
			return false;

		if(!app->indices.handle)
			return false;
	}

	return true;
}


void update_matrices(VulkanApp* app, vec3s position, float angle) noexcept
{
	vec3s axis = { 1.0f, 0.3f, 0.5f };

    mat4s model = glms_translate(glms_mat4_identity(), position);
    model       = glms_rotate(model, glm_rad(angle), axis);
    mat4s modelView  = app->camera.getViewMatrix();
    mat4s projection = glms_perspective(glm_rad(60.f), app->width / (float)app->height, 0.1f, 100.f);

    app->modelViewProjectionMatrix = glms_mat4_mul(glms_mat4_mul(projection, modelView), model);
}


void write_command_buffer(VulkanApp* app, VkCommandBuffer cmd, VkDescriptorSet descriptorSet) noexcept
{
    VkDeviceSize offsets[] = {0};
    VkBuffer vertexBuffers[] = {app->vertices.handle};

    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(cmd, app->indices.handle, 0, VK_INDEX_TYPE_UINT32);
    vkCmdPushConstants(cmd, app->pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4s), app->modelViewProjectionMatrix.raw);
    vkCmdDrawIndexed(cmd, app->indices.size, 1, 0, 0, 0);
}


void draw_frame(VulkanApp* app) noexcept
{
    uint32_t frame  = app->sync.currentFrame;
    VkDevice device = app->context.device;
    VkQueue  queue  = app->context.queue;

    VkResult result = vkWaitForFences(device, 1, &app->sync.inFlightFences[frame], VK_TRUE, UINT64_MAX);

	if (result != VK_SUCCESS)
    {
#ifdef DEBUG
        printf("failed to wait for fences!\n");
#endif
		return;
    }

    uint32_t imageIndex;
    result = vkAcquireNextImageKHR(device, app->view.swapchain, UINT64_MAX, app->sync.imageAvailableSemaphores[frame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        vkDeviceWaitIdle(app->context.device);
		app->view.recreate(true);

        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
#ifdef DEBUG
        printf("failed to acquire swap chain image!\n");
#endif
		return;
    }

    result = vkResetFences(device, 1, &app->sync.inFlightFences[frame]);

	if (result != VK_SUCCESS)
    {
#ifdef DEBUG
        printf("failed to reset fences!\n");
#endif
		return;
    }

    VkCommandBuffer commandBuffer = app->commandPool.commandBuffers[frame];
    VkDescriptorSet descriptorSet = app->descriptorSets[frame];

    result = vkResetCommandBuffer(commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);

	if (result != VK_SUCCESS)
    {
#ifdef DEBUG
        printf("failed to reset command buffers!\n");
#endif
		return;
    }

    if(!app->renderer.begin(commandBuffer, &app->view, imageIndex))
        return;

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, app->pipeline.handle);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, app->pipeline.layout, 0, 1, &descriptorSet, 0, VK_NULL_HANDLE);

    for (uint32_t i = 0; i < 10; ++i)
    {
        const float angle = 20.f * i;
        update_matrices(app, cubePositions[i], angle);
        write_command_buffer(app, commandBuffer, descriptorSet);
    }

    if(!app->renderer.end(commandBuffer, &app->view, imageIndex))
        return;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	
    const VkSubmitInfo submitInfo = 
	{
		.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext                = VK_NULL_HANDLE,
		.waitSemaphoreCount   = 1,
		.pWaitSemaphores      = app->sync.imageAvailableSemaphores.data() + frame,
		.pWaitDstStageMask    = waitStages,
		.commandBufferCount   = 1,
		.pCommandBuffers      = &app->commandPool.commandBuffers[frame],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores    = &app->sync.renderFinishedSemaphores[frame]
	};

	result = vkQueueSubmit(queue, 1, &submitInfo, app->sync.inFlightFences[frame]);

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
		.pWaitSemaphores    = &app->sync.renderFinishedSemaphores[frame],
		.swapchainCount     = 1,
		.pSwapchains        = &app->view.swapchain,
		.pImageIndices      = &imageIndex,
		.pResults           = VK_NULL_HANDLE
	};

    result = vkQueuePresentKHR(queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || app->framebufferResized)
    {
        app->framebufferResized = false;
        vkDeviceWaitIdle(app->context.device);
		app->view.recreate(true);
    }
    else if (result != VK_SUCCESS)
    {
#ifdef DEBUG
        printf("failed to present swap chain image!!\n");
#endif
		return;
    }

    app->sync.currentFrame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;
}