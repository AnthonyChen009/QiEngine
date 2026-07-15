#include "renderer/vulkan/VulkanIndexBuffer.hpp"
#include "renderer/vulkan/VulkanBuffer.hpp"
#include "renderer/vulkan/VulkanCommands.hpp"
#include <vulkan/vulkan_core.h>

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
bool VulkanIndexBuffer::isBufferValid() const {
    return m_buffer.getBuffer() != VK_NULL_HANDLE;
}

void VulkanIndexBuffer::bind(CommandBufferHandle commandBuffer) const {
    VkCommandBuffer vkCmdBuffer = static_cast<VkCommandBuffer>(commandBuffer);
    vkCmdBindIndexBuffer(
        vkCmdBuffer,
        m_buffer.getBuffer(),
        0,
        VK_INDEX_TYPE_UINT32
    );
}

VulkanIndexBuffer::~VulkanIndexBuffer() {
    m_buffer.destroy();
}

}
