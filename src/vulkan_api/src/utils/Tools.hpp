#ifndef VULKAN_TOOLS_HPP
#define VULKAN_TOOLS_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <span>

#include <vulkan/vulkan.h>


#define BEGIN_NAMESPACE_VKTOOLS namespace vktools {
#define END_NAMESPACE_VKTOOLS }


BEGIN_NAMESPACE_VKTOOLS


uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice gpu) noexcept;


VkCommandBuffer begin_single_time_commands(VkDevice device, VkCommandPool pool) noexcept;
void end_single_time_commands(VkCommandBuffer cmd, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept;


VkBuffer create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory* bufferMemory, VkDevice device, VkPhysicalDevice gpu) noexcept;
void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept;


// Images
bool transition_image_layout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool pool) noexcept;
bool copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool pool) noexcept;
VkImage create_image_2D(VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory* imageMemory) noexcept;
VkImageView create_image_view_2D(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) noexcept;


VkFormat find_supported_format(std::span<const VkFormat> formats, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice gpu) noexcept;
bool has_stencil_component(VkFormat format) noexcept;


END_NAMESPACE_VKTOOLS

#endif // !VULKAN_TOOLS_HPP