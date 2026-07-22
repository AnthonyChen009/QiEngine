#include "VulkanDevice.hpp"
#include "renderer/utils/VulkanUtils.hpp"
#include "core/Assert.hpp"
#include <set>
namespace Qi {

VulkanDevice::VulkanDevice(VkInstance instance, VkSurfaceKHR surface)
    : m_instance(instance), m_surface(surface) {
    pickPhysicalDevice();
    createLogicalDevice();
}

VulkanDevice::~VulkanDevice() {
    if (m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }
}

void VulkanDevice::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    QI_RENDERER_ASSERT(deviceCount > 0, "Failed to find GPUs with Vulkan support!");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    int bestScore = -1;

    for (const VkPhysicalDevice& device : devices) {
        if (!isDeviceSuitable(device))
            continue;

        int score = rateDevice(device);
        if (score > bestScore) {
            bestScore = score;
            bestDevice = device;
        }
    }

    QI_RENDERER_ASSERT(bestDevice != VK_NULL_HANDLE && bestScore > 0, "Failed to find suitable GPU!");
    m_physicalDevice = bestDevice;

    m_rtSupported = checkRayTracingSupport(m_physicalDevice);
    QI_CORE_INFO(m_rtSupported ? "Ray tracing supported on selected GPU" : "Ray tracing NOT supported on selected GPU — RT pipeline disabled");
}

bool VulkanDevice::checkRayTracingSupport(VkPhysicalDevice device) {
    uint32_t extCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> available(extCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, available.data());

    for (const char* required : m_rtDeviceExtensions) {
        bool found = std::any_of(available.begin(), available.end(),
            [&](const VkExtensionProperties& ext) {
                return strcmp(ext.extensionName, required) == 0;
            });
        if (!found) return false;
    }

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeatures{};
    rtPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;

    VkPhysicalDeviceAccelerationStructureFeaturesKHR asFeatures{};
    asFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    asFeatures.pNext = &rtPipelineFeatures;

    VkPhysicalDeviceBufferDeviceAddressFeatures bdaFeatures{};
    bdaFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    bdaFeatures.pNext = &asFeatures;

    VkPhysicalDeviceFeatures2 features2{};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &bdaFeatures;

    vkGetPhysicalDeviceFeatures2(device, &features2);

    return rtPipelineFeatures.rayTracingPipeline && asFeatures.accelerationStructure && bdaFeatures.bufferDeviceAddress;
}

void VulkanDevice::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamily.value(),
        indices.presentFamily.value()
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.shaderInt64 = VK_TRUE;

    VkPhysicalDeviceScalarBlockLayoutFeatures scalarBlockLayoutFeatures{};
    scalarBlockLayoutFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES;
    scalarBlockLayoutFeatures.scalarBlockLayout = VK_TRUE;

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{};
    indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexingFeatures.runtimeDescriptorArray = VK_TRUE;
    indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
    indexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;

    //rt
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeatures{};
    rtPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    rtPipelineFeatures.rayTracingPipeline = VK_TRUE;

    VkPhysicalDeviceAccelerationStructureFeaturesKHR asFeatures{};
    asFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    asFeatures.accelerationStructure = VK_TRUE;

    VkPhysicalDeviceBufferDeviceAddressFeatures bdaFeatures{};
    bdaFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    bdaFeatures.bufferDeviceAddress = VK_TRUE;

    std::vector<const char*> enabledExtensions = m_deviceExtensions;
    if (m_rtSupported) {
        enabledExtensions.insert(enabledExtensions.end(), m_rtDeviceExtensions.begin(), m_rtDeviceExtensions.end());
        bdaFeatures.pNext = &asFeatures;
        asFeatures.pNext = &rtPipelineFeatures;
        scalarBlockLayoutFeatures.pNext = &bdaFeatures;
        indexingFeatures.pNext = &scalarBlockLayoutFeatures;
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &indexingFeatures; // chain it
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    if constexpr (VulkanUtils::enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanUtils::validationLayers.size());
        createInfo.ppEnabledLayerNames = VulkanUtils::validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult res = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    QI_RENDERER_ASSERT(res == VK_SUCCESS, "Failed to create logical device!");

    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device) const {
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = VulkanUtils::querySwapChainSupport(device, m_surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

int VulkanDevice::rateDevice(VkPhysicalDevice device) const {
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceFeatures features;

    vkGetPhysicalDeviceProperties(device, &props);
    vkGetPhysicalDeviceFeatures(device, &features);

    if (!features.geometryShader)
        return 0;

    int score = 0;

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    score += props.limits.maxImageDimension2D;

    return score;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) const {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const VkExtensionProperties& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) const {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    uint32_t i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (!indices.graphicsFamily.has_value() && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

        if (!indices.presentFamily.has_value() && presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

}
