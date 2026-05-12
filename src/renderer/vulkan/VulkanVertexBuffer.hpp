#pragma once

#include "VulkanBuffer.hpp"
#include <vector>
#include "renderer/types/Vertex.hpp"

namespace Qi {

//template<typename T>
class VulkanVertexBuffer {
public:
    VulkanVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, const std::vector<Vertex>& vertices);
    ~VulkanVertexBuffer();
    void bind(VkCommandBuffer commandBuffer);

private:
    VulkanBuffer m_buffer;
};

}
