#pragma once
#include <vulkan/vulkan.h>
#include <string>

namespace Qi {

class VulkanTexture {
public:
    VulkanTexture(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkCommandPool commandPool,
        VkQueue graphicsQueue,
        const std::string& path
    );
    ~VulkanTexture();

    VkImageView getImageView() const;
    VkSampler getSampler() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

}
