#pragma once

#include <vulkan/vulkan.h>

namespace Qi::VulkanCommands {

VkCommandBuffer beginSingleTimeCommands(
    VkDevice device,
    VkCommandPool commandPool
);

void endSingleTimeCommands(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkCommandBuffer commandBuffer
);

void copyBuffer(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size
);

void transitionImageLayout(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkImage image,
    VkFormat format,
    VkImageLayout oldLayout,
    VkImageLayout newLayout
);

}
