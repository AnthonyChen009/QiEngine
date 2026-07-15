#pragma once
#include "renderer/VertexBuffer.hpp"
#include "VulkanBuffer.hpp"
#include "types/Vertex.hpp"
#include <vector>

namespace Qi {

class VulkanVertexBuffer : public VertexBuffer {
public:
    VulkanVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, const std::vector<Vertex>& vertices);
    ~VulkanVertexBuffer() override;

    void bind(CommandBufferHandle commandBuffer) const override;
    bool isBufferValid() const override;

private:
    VulkanBuffer m_buffer;
};

}
