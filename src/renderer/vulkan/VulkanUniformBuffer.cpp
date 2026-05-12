#include "VulkanUniformBuffer.hpp"

namespace Qi {

VulkanUniformBuffer::VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size) : m_buffer(device, physicalDevice) {
    m_buffer.create(
        size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
    m_buffer.destroy();
}

void VulkanUniformBuffer::setData(const void* data, VkDeviceSize size) {
    m_buffer.setData(data, size);
}

}
