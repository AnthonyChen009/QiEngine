#include "VulkanPlatform.hpp"

#include "Assert.hpp"
#include <GLFW/glfw3.h>

namespace Qi::VulkanPlatform {

const char**  getRequiredVulkanExtensions(uint32_t &count) {
    return glfwGetRequiredInstanceExtensions(&count);
}

VkSurfaceKHR createVulkanSurface(VkInstance instance, Window& window) {
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    auto* nativeWindow = static_cast<GLFWwindow*>(window.getNativeWindow());
    VkResult result = glfwCreateWindowSurface(instance, nativeWindow, nullptr, &surface);

    QI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan surface!");

    return surface;
}

}
