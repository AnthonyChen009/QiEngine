#include "VulkanVertexBuffer.hpp"

namespace Qi {
VulkanVertexBuffer::VulkanVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, const std::vector<Vertex>& vertices) : m_buffer(device, physicalDevice) {
    VkDeviceSize size = sizeof(Vertex) * vertices.size();

    m_buffer.create(
        size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    m_buffer.setData(vertices.data(), size);
}

void VulkanVertexBuffer::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = { m_buffer.getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

VulkanVertexBuffer::~VulkanVertexBuffer() {
    m_buffer.destroy();
}

}
