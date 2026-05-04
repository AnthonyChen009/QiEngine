#pragma once

#include "Window.hpp"


#include <vulkan/vulkan.h>

namespace Qi::VulkanPlatform {

const char** getRequiredVulkanExtensions(uint32_t &count);
VkSurfaceKHR createVulkanSurface(VkInstance instance, Window& window);
void getFrameBufferSize(Window& window, int& width, int& height);
}
