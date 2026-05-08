#pragma once
#include <vulkan/vulkan.h>

namespace Qi {

class VulkanRenderPass {
public:
    VulkanRenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat swapChainImageFormat);
    ~VulkanRenderPass();

    VkRenderPass getRenderPass() const;

private:
    void createRenderPass(VkFormat swapChainImageFormat);

    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
};

}
