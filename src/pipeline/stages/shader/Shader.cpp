#include <cstdio>
#include <memory>
#include <vector>

#include "pipeline/stages/shader/Shader.hpp"


struct FileCloser
{
    void operator()(FILE* f) const noexcept
    {
        if (f)
            fclose(f);
    }
};

using FileOpener = std::unique_ptr<FILE, FileCloser>;



Shader::Shader(VkDevice device) noexcept :
    m_device(device),
    m_module(nullptr),
    m_stage(VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
{
}


Shader::~Shader()
{
    if (m_module)
        vkDestroyShaderModule(m_device, m_module, nullptr);
}


bool Shader::loadFromFile(const std::filesystem::path& filePath, VkShaderStageFlagBits stage) noexcept
{
    if (m_module)
    {
        vkDestroyShaderModule(m_device, m_module, nullptr);
        m_module = nullptr;
        m_stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    }

    namespace fs = std::filesystem;
    std::error_code ec;

    if (!fs::exists(filePath, ec))
        return false;

    FileOpener fileOpener(fopen(filePath.string().c_str(), "rb"));

    if (!fileOpener)
        return false;

    auto* fp = fileOpener.get();
    
    fseek(fp, 0, SEEK_END);
    size_t fileSize = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::vector<std::byte> byteCode(fileSize);
    size_t bytesRead = fread(byteCode.data(), 1, fileSize, fp);

    if (bytesRead != fileSize)
        return false;

    const VkShaderModuleCreateInfo createInfo = 
    {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = 0,
        .codeSize = byteCode.size(),
        .pCode    = reinterpret_cast<const uint32_t*>(byteCode.data())
    };

    VkShaderModule shaderModule;

    if (vkCreateShaderModule(m_device, &createInfo, NULL, &shaderModule) == VK_SUCCESS)
    {
        m_module = shaderModule;
        m_stage  = stage;

        return true;
    }

    return false;
}


VkPipelineShaderStageCreateInfo Shader::operator()() const noexcept
{
    return
        {.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
         .pNext               = nullptr,
         .flags               = 0,
         .stage               = m_stage,
         .module              = m_module,
         .pName               = "main",
         .pSpecializationInfo = nullptr};
}