#pragma once
#include <vulkan/vulkan.h>

namespace Qi::VulkanSampler {

VkSampler create(VkDevice device, VkPhysicalDevice physicalDevice);
void destroy(VkDevice device, VkSampler sampler);


}
