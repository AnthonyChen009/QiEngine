#pragma once

#include "VulkanBuffer.hpp"
#include <vector>


namespace Qi {

//template<typename T>
class VulkanIndexBuffer {
public:
    VulkanIndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, const std::vector<uint32_t>& indices);
    ~VulkanIndexBuffer();
    void bind(VkCommandBuffer commandBuffer);
    uint32_t getCount() const { return m_count; }
private:
    VulkanBuffer m_buffer;
    uint32_t m_count = 0;
};

}
