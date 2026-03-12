#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "pipeline/stages/shader/Shader.hpp"


// Helper function to read a file into a buffer
static size_t read_shader_file(const char* filename, char** buffer) noexcept;

// Function to create a shader module from SPIR-V data
static VkShaderModule create_shader_module(VkDevice device, const char* filename) noexcept;


Shader::Shader(VkDevice device) noexcept:
    m_device(device),
    m_module(VK_NULL_HANDLE),
    m_stage(VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
{
    
}


Shader::~Shader()
{
    if(m_module)
        vkDestroyShaderModule(m_device, m_module, VK_NULL_HANDLE);
}


bool Shader::loadFromFile(const char* filePath, VkShaderStageFlagBits stage) noexcept
{
    if(auto shaderModule = create_shader_module(m_device, filePath))
    {
        m_module = shaderModule;
        m_stage = stage;

        return true;
    }
        
    return false;
}


VkPipelineShaderStageCreateInfo Shader::getInfo() const noexcept
{
    const VkPipelineShaderStageCreateInfo info =
    {
        .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext               = VK_NULL_HANDLE,
        .flags               = 0,
        .stage               = m_stage,
        .module              = m_module,
        .pName               = "main",
        .pSpecializationInfo = VK_NULL_HANDLE
    };

    return info;
}



size_t read_shader_file(const char* filename, char** buffer) noexcept
{
    FILE* file = fopen(filename, "rb");

    if (!file) 
    {
#ifdef DEBUG
        fprintf(stderr, "Failed to open file: %s\n", filename);
#endif
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = (char*)malloc(fileSize);

    if (!*buffer) 
    {
        fclose(file);
#ifdef DEBUG
        fprintf(stderr, "Failed to allocate memory for shader file: %s\n", filename);
#endif
        return 0;
    }

    size_t bytesRead = fread(*buffer, 1, fileSize, file);

    if (bytesRead != fileSize) 
    {
        free(*buffer);
        fclose(file);
#ifdef DEBUG
        fprintf(stderr, "Failed to read full file: %s\n", filename);
#endif
        return 0;
    }

    fclose(file);

    return fileSize;
}


VkShaderModule create_shader_module(VkDevice device, const char* filename) noexcept
{
    char* code;
    size_t codeSize = read_shader_file(filename, &code);

    if (codeSize == 0)
        return VK_NULL_HANDLE;

    const VkShaderModuleCreateInfo createInfo = 
    {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = VK_NULL_HANDLE,
        .flags    = 0,
        .codeSize = codeSize,             // The pointer must be a uint32_t pointer and aligned correctly.
        .pCode    = (const uint32_t*)code // Ensure your memory allocation is suitable for this (malloc usually is).
    };

    VkShaderModule shaderModule;

    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) 
    {
#ifdef DEBUG
        fprintf(stderr, "Failed to create shader module from file: %s\n", filename);
#endif
        shaderModule = VK_NULL_HANDLE;
    }

    free(code);

    return shaderModule;
}