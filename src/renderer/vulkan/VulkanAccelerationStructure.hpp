#pragma once
#include <vulkan/vulkan.h>
#include "renderer/vulkan/VulkanBuffer.hpp"

namespace Qi {

class VulkanAccelerationStructure {
public:
    VulkanAccelerationStructure(VkDevice device, VkPhysicalDevice physicalDevice);
    ~VulkanAccelerationStructure();

    // Builds a BLAS from a vertex/index buffer pair.
    // allowUpdate: reserved for future skeletal refit support. Defaults to false
    // for static meshes today; skinned meshes will pass true once skinning exists.
    void buildBLAS(VkCommandPool commandPool, VkQueue queue, const VulkanBuffer& vertexBuffer, uint32_t vertexCount, VkDeviceSize vertexStride, const VulkanBuffer& indexBuffer, uint32_t indexCount, bool allowUpdate = false);
    void buildTLAS(VkCommandPool commandPool, VkQueue queue, const VulkanBuffer& instanceBuffer, uint32_t instanceCount, bool allowUpdate = false);

    VkAccelerationStructureKHR getHandle() const { return m_accelerationStructure; }
    VkDeviceAddress getDeviceAddress() const { return m_deviceAddress; }

    void destroy();

private:
    VkDevice m_device;
    VkPhysicalDevice m_physicalDevice;

    VkAccelerationStructureKHR m_accelerationStructure = VK_NULL_HANDLE;
    VulkanBuffer m_asBuffer;
    VkDeviceAddress m_deviceAddress = 0;

    bool m_allowsUpdate = false;
};

}
