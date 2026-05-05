#pragma once

#include "VulkanBuffer.hpp"
#include <vulkan/vulkan.h>

namespace Qi {

class VulkanUniformBuffer {
public:
    VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size);
    ~VulkanUniformBuffer();

    void setData(const void* data, VkDeviceSize size);

    VkBuffer getBuffer() const { return m_buffer.getBuffer(); }

private:
    VulkanBuffer m_buffer;
};

}
