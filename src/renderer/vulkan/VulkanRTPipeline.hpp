#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "renderer/vulkan/VulkanBuffer.hpp"

namespace Qi {

class VulkanRTPipeline {
public:
    VulkanRTPipeline(VkDevice device);
    ~VulkanRTPipeline();

    VkPipeline getPipeline() const;
    VkPipelineLayout getPipelineLayout() const;
    VkDescriptorSetLayout getDescriptorSetLayout() const;

    // Needed for SBT construction later — the pipeline knows how many
    // shader groups it has and in what order (raygen, miss, hit).
    uint32_t getShaderGroupCount() const { return static_cast<uint32_t>(m_shaderGroups.size()); }

    void buildSBT(VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue);

    VkStridedDeviceAddressRegionKHR getRaygenRegion() const { return m_raygenRegion; }
    VkStridedDeviceAddressRegionKHR getMissRegion() const { return m_missRegion; }
    VkStridedDeviceAddressRegionKHR getHitRegion() const { return m_hitRegion; }
    void cmdTraceRays(VkCommandBuffer commandBuffer, uint32_t width, uint32_t height);

private:
    void createDescriptorSetLayout();
    void createRTPipeline();
    VkShaderModule createShaderModule(const std::vector<char>& code);

    VkDevice m_device = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkRayTracingShaderGroupCreateInfoKHR> m_shaderGroups;

    std::unique_ptr<VulkanBuffer> m_sbtBuffer;
    VkStridedDeviceAddressRegionKHR m_raygenRegion{};
    VkStridedDeviceAddressRegionKHR m_missRegion{};
    VkStridedDeviceAddressRegionKHR m_hitRegion{};
};

}
