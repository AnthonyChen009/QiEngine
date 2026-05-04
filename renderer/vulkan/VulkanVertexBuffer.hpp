#pragma once

#include "VulkanBuffer.hpp"
#include <vector>
#include "Vertex.hpp"

namespace Qi {

//template<typename T>
class VulkanVertexBuffer {
public:
    VulkanVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, const std::vector<Vertex>& vertices);
    ~VulkanVertexBuffer();
    void bind(VkCommandBuffer commandBuffer);

private:
    VulkanBuffer m_buffer;
};

}
