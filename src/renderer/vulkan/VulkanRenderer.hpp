#pragma once

#include <unordered_map>
#include "renderer/RendererBackend.hpp"
#include "core/Window.hpp"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan_core.h>
#include <optional>
#include "VulkanVertexBuffer.hpp"
#include "VulkanIndexBuffer.hpp"
#include "VulkanUniformBuffer.hpp"
#include "renderer/types/SkyUbo.hpp"
#include "renderer/types/UniformBufferObject.hpp"
#include "renderer/vulkan/VulkanDevice.hpp"
#include "renderer/vulkan/VulkanGraphicsPipeline.hpp"
#include "renderer/vulkan/VulkanInstance.hpp"
#include "renderer/utils/VulkanUtils.hpp"
#include "renderer/vulkan/VulkanRenderPass.hpp"
#include "renderer/vulkan/VulkanSurface.hpp"
#include "renderer/vulkan/VulkanSwapChain.hpp"
#include "renderer/vulkan/VulkanTexture.hpp"

namespace Qi {

class VulkanRenderer : public RendererBackend {
public:
    void init(Window& window) override;
    void shutdown() override;

    bool beginFrame() override;
    void endFrame() override;

    void onWindowResize(uint32_t width, uint32_t height) override;
    void onVysncToggle(bool isVSync) override;
    std::shared_ptr<Texture2D> getOrLoadTexture(const std::string& path) override;
public:
    void drawIndexed(uint32_t count) override;
    void updateUniformBuffer2D() override;
    void updateUniformBuffer3D(UniformBufferObject& ubo) override;
    void updateUniformBufferSky(SkyUniformBufferObject& ubo) override;
    void pushConstants2D(const PushConstant& push) override;
    void pushConstants3D(const PushConstant& push) override;
    void initImGui(Window* window) override;
    void shutdownImGui() override;
    void beginImGuiFrame() override;
    void renderImGui() override;
    void bindPipeline(VulkanUtils::PipelineType type) override;
    void bindBuffers(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer) override;
    void drawFullscreenTriangle() override;

    std::shared_ptr<VertexBuffer> createVertexBuffer(const std::vector<Vertex>& vertices) override;
    std::shared_ptr<IndexBuffer> createIndexBuffer(const std::vector<uint32_t>& indices) override;

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
    void createDescriptorSets(VulkanUtils::PipelineType type);
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
    void createImGuiDescriptorPool();


private:
    VulkanInstance m_instance;

    std::optional<VulkanSurface> m_surface;

    std::optional<VulkanDevice> m_vulkanDevice;

    std::optional<VulkanSwapChain> m_swapChain;

    std::optional<VulkanRenderPass> m_renderPass;

    std::optional<VulkanGraphicsPipeline> m_graphicsPipeline2D;
    std::optional<VulkanGraphicsPipeline> m_graphicsPipeline3D;
    std::optional<VulkanGraphicsPipeline> m_graphicsPipelineSky;

    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    VkImage m_depthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_depthImageMemory = VK_NULL_HANDLE;
    VkImageView m_depthImageView = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> m_swapChainFrameBuffers;

    std::unordered_map<std::string, std::shared_ptr<VulkanTexture>> m_textureCache;

    const VertexBuffer* m_boundVertexBuffer = nullptr;
    const IndexBuffer* m_boundIndexBuffer = nullptr;

    std::vector<std::unique_ptr<VulkanUniformBuffer>> m_uniformBuffers2D;
    std::vector<std::unique_ptr<VulkanUniformBuffer>> m_uniformBuffers3D;
    std::vector<std::unique_ptr<VulkanUniformBuffer>> m_skyUniformBuffers;

    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_descriptorSets2D;
    std::vector<VkDescriptorSet> m_descriptorSets3D;
    std::vector<VkDescriptorSet> m_descriptorSetsSky;

    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

    const int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t m_currentImageIndex = 0;
    uint32_t m_currentFrame = 0;
    bool m_pipelineBound = false;
    bool m_frameBufferResized = false;
    Window* m_window = nullptr;
    uint32_t m_winWidth = 0;
    uint32_t m_winHeight = 0;
    uint32_t m_nextTextureIndex = 0;
    VkDescriptorPool m_imguiDescriptorPool = VK_NULL_HANDLE;
    bool m_pendingVSync = false;
    bool m_vsyncTogglePending = false;
};

}
