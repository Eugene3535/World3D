#include <cglm/struct/affine-pre.h>

#include "resources/ResourceManager.hpp"
#include "view/swapchain/Swapchain.hpp"
#include "view/View.hpp"
#include "pipeline/descriptors/DescriptorSetLayout.hpp"
#include "pipeline/state/PipelineState.hpp"
#include "scenes/root/RootScene.hpp"


RootScene::RootScene(const Scene* parent) noexcept:
    Scene(parent)
{

}


RootScene::~RootScene()
{
	m_bufferHolder.destroy();
	m_texture.destroy();
	m_descriptorPool.destroy();
	m_pipeline.destroy();
}


bool RootScene::create() noexcept
{
	VkDevice device = vkContext->getLogicalDevice();

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

    auto commandPoolHandle = static_cast<VkCommandPool>(vkResource->getObjectByType(VK_OBJECT_TYPE_COMMAND_POOL));

    if (!commandPoolHandle)
        return false;

    {
        m_uniformBuffers.resize(vkView->getSwapchain()->getImageCount());
        std::array<mat4s, 1> identity = { glms_mat4_identity() };

        for (size_t i = 0; i < m_uniformBuffers.size(); ++i) 
            m_uniformBuffers[i] = m_bufferHolder.allocate<mat4s>(identity,
                                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                                                                 commandPoolHandle);
    }

	{
        if(!m_texture.loadFromFile("res/textures/container.jpg", commandPoolHandle))
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

		m_vertexBuffer = m_bufferHolder.allocate<float>(vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, commandPoolHandle);
		m_indexBuffer = m_bufferHolder.allocate<uint32_t>(indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, commandPoolHandle);

		if (!m_vertexBuffer.handle)
			return false;

		if (!m_indexBuffer.handle)
			return false;
	}

	return true;
}