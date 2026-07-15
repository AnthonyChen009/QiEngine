#pragma once

#include "renderer/IndexBuffer.hpp"
#include "VulkanBuffer.hpp"
#include <vector>

namespace Qi {

class VulkanIndexBuffer : public IndexBuffer {
public:
    VulkanIndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, const std::vector<uint32_t>& indices);
    ~VulkanIndexBuffer() override;

    void bind(CommandBufferHandle commandBuffer) const override;
    uint32_t getCount() const override { return m_count; }
    bool isBufferValid() const override;

private:
    VulkanBuffer m_buffer;
    uint32_t m_count = 0;
};

}
