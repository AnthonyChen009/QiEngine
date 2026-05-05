#include "vulkan/VulkanIndexBuffer.hpp"
#include "vulkan/VulkanBuffer.hpp"
#include "vulkan/VulkanCommands.hpp"

namespace Qi {

VulkanIndexBuffer::VulkanIndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, const std::vector<uint32_t>& indices) : m_buffer(device, physicalDevice) {
    m_count = static_cast<uint32_t>(indices.size());

    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();
    VulkanBuffer stagingBuffer(device, physicalDevice);

    stagingBuffer.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.setData(indices.data(), bufferSize);

    m_buffer.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VulkanCommands::copyBuffer(
        device,
        commandPool,
        graphicsQueue,
        stagingBuffer.getBuffer(),
        m_buffer.getBuffer(),
        bufferSize
    );

}

void VulkanIndexBuffer::bind(VkCommandBuffer commandBuffer) {
    vkCmdBindIndexBuffer(
        commandBuffer,
        m_buffer.getBuffer(),
        0,
        VK_INDEX_TYPE_UINT32
    );
}

VulkanIndexBuffer::~VulkanIndexBuffer() {
    m_buffer.destroy();
}

}
