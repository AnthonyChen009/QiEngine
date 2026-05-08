#pragma once

#include "RendererBackend.hpp"
#include "Window.hpp"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan_core.h>
#include <optional>
#include "VulkanVertexBuffer.hpp"
#include "VulkanIndexBuffer.hpp"
#include "VulkanUniformBuffer.hpp"
#include "vulkan/VulkanDevice.hpp"
#include "vulkan/VulkanGraphicsPipeline.hpp"
#include "vulkan/VulkanInstance.hpp"
#include "utils/VulkanUtils.hpp"
#include "vulkan/VulkanRenderPass.hpp"
#include "vulkan/VulkanSurface.hpp"
#include "vulkan/VulkanSwapChain.hpp"

namespace Qi {

class VulkanRenderer : public RendererBackend {
public:
    void init(Window& window) override;
    void shutdown() override;

    bool beginFrame() override;
    void endFrame() override;

    void onWindowResize(uint32_t width, uint32_t height) override;
public:
    void drawIndexed(uint32_t count) override;
    void updateUniformBuffer() override;
    void pushConstants(const PushConstant2D& push) override;
private:
    void createInstance(const std::string& appName);
    void pickPhysicalDevice();
    int rateDevice(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,  Window& window);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void createDescriptorSets();
    void createFrameBuffers();
    void createCommandPool();
    void createCommandBuffers();
    void beginCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void createSyncObjects();
    void recreateSwapChain();
    void cleanupSwapChain();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDepthResources();
    bool hasStencilComponent(VkFormat format);
    void createTextureImageView();
    void bindPipeline() override;

private:
    const int MAX_FRAMES_IN_FLIGHT = 2;
    VulkanInstance m_instance;
    std::optional<VulkanSurface> m_surface;
    std::optional<VulkanDevice> m_vulkanDevice;
    std::optional<VulkanSwapChain> m_swapChain;
    std::optional<VulkanRenderPass> m_renderPass;

    std::optional<VulkanGraphicsPipeline> m_graphicsPipeline;

    std::vector<VkFramebuffer> m_swapChainFrameBuffers;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    uint32_t m_currentImageIndex = 0;
    bool m_pipelineBound = false;
    uint32_t m_currentFrame = 0;
    bool m_frameBufferResized = false;
    Window* m_window = nullptr;
    uint32_t m_winHeight = 0;
    uint32_t m_winWidth = 0;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    std::unique_ptr<VulkanVertexBuffer> m_vertexBuffer;
    std::unique_ptr<VulkanIndexBuffer> m_indexBuffer;
    std::vector<std::unique_ptr<VulkanUniformBuffer>> m_uniformBuffers;

    std::vector<VkDescriptorSet> m_descriptorSets;
    VkImage m_depthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_depthImageMemory = VK_NULL_HANDLE;
    VkImageView m_depthImageView = VK_NULL_HANDLE;
    VkImage m_textureImage = VK_NULL_HANDLE;
    VkDeviceMemory m_textureImageMemory = VK_NULL_HANDLE;
    VkImageView m_textureImageView = VK_NULL_HANDLE;
    VkSampler m_textureSampler = VK_NULL_HANDLE;
};

}
