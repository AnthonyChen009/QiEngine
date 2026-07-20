#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "renderer/types/QueueFamilyIndices.hpp"
namespace Qi {

class VulkanDevice {
public:
    VulkanDevice(VkInstance instance, VkSurfaceKHR surface);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    VkDevice getDevice() const { return m_device; }
    VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
    VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
    VkQueue getPresentQueue() const { return m_presentQueue; }
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
    bool hasRTSupport() { return m_rtSupported; }

private:
    void pickPhysicalDevice();
    bool checkRayTracingSupport(VkPhysicalDevice device);
    void createLogicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device) const;
    int rateDevice(VkPhysicalDevice device) const;
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
    VkInstance m_instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

    const std::vector<const char*> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
    };

    std::vector<const char*> m_rtDeviceExtensions = {
        VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
        VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
    };

private:
    bool m_rtSupported = false;
};

}
