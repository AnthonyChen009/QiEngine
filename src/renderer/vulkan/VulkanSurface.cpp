#include "VulkanSurface.hpp"

#include "platform/graphicsAPI/VulkanPlatform.hpp"

namespace Qi {

VulkanSurface::VulkanSurface(VkInstance instance, Window &window) : m_instance(instance){
    m_surface = VulkanPlatform::createVulkanSurface(instance, window);
}

VulkanSurface::~VulkanSurface() {
    if (m_instance != VK_NULL_HANDLE && m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
}

VkSurfaceKHR VulkanSurface::getVkSurface() {
    return m_surface;
}

}
