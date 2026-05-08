#pragma once

#include <vulkan/vulkan.h>
#include "Window.hpp"

namespace Qi {

class VulkanSurface {

public:
    VulkanSurface(VkInstance instance, Window &window);
    ~VulkanSurface();
    VkSurfaceKHR getVkSurface();
private:

private:
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkInstance m_instance = VK_NULL_HANDLE;
};

}
