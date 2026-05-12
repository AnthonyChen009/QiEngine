#include "VulkanVertexBuffer.hpp"
#include "renderer/vulkan/VulkanBuffer.hpp"
#include "renderer/vulkan/VulkanCommands.hpp"

namespace Qi {
VulkanVertexBuffer::VulkanVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, const std::vector<Vertex>& vertices) : m_buffer(device, physicalDevice) {
    VkDeviceSize size = sizeof(Vertex) * vertices.size();

    VulkanBuffer stagingBuffer(device, physicalDevice);

    stagingBuffer.create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.setData(vertices.data(), size);


    m_buffer.create(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VulkanCommands::copyBuffer(
        device,
        commandPool,
        graphicsQueue,
        stagingBuffer.getBuffer(),
        m_buffer.getBuffer(),
        size
    );
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
