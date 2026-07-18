#include "VulkanInstance.hpp"
#include "core/Log.hpp"
#include "core/Assert.hpp"
#include "platform/graphicsAPI/VulkanPlatform.hpp"
#include <vulkan/vulkan_core.h>
#include "renderer/utils/VulkanUtils.hpp"

namespace Qi {

static bool checkValidationLayerSupport() {
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : VulkanUtils::validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (std::strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData
) {
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        QI_CORE_ERROR("Vulkan validation: {0}", callbackData->pMessage);
    } else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        QI_CORE_WARN("Vulkan validation: {0}", callbackData->pMessage);
    } else {
        QI_CORE_INFO("Vulkan validation: {0}", callbackData->pMessage);
    }

    return VK_FALSE;
}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = vulkanDebugCallback;
}

static VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* debugMessenger
) {
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
    );

    if (func) {
        return func(instance, createInfo, allocator, debugMessenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* allocator
) {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
    );

    if (func) {
        func(instance, debugMessenger, allocator);
    }
}

VulkanInstance::VulkanInstance() {

}

VulkanInstance::~VulkanInstance() {
    if constexpr (VulkanUtils::enableValidationLayers) {
        if (m_debugMessenger != VK_NULL_HANDLE) {
            destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
            m_debugMessenger = VK_NULL_HANDLE;
        }
    }
    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

void VulkanInstance::createVkInstance(const std::string& appName) {
    uint32_t instanceVersion = VK_API_VERSION_1_0; // safe default

    if (vkEnumerateInstanceVersion) {
        vkEnumerateInstanceVersion(&instanceVersion);
    }

    uint32_t targetVersion;
    if (instanceVersion >= VK_API_VERSION_1_4) {
        targetVersion = VK_API_VERSION_1_4;
    }
    else if (instanceVersion >= VK_API_VERSION_1_3) {
        targetVersion = VK_API_VERSION_1_3;
    }
    else if (instanceVersion >= VK_API_VERSION_1_2) {
        targetVersion = VK_API_VERSION_1_2;
    }
    else {
        QI_RENDERER_ASSERT(false, "Vulkan 1.2 is not supported by this system!");
    }


    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "Qi Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = targetVersion;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if constexpr (VulkanUtils::enableValidationLayers) {
        QI_RENDERER_ASSERT(checkValidationLayerSupport(), "Validation layers requested, but not available!");

        createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanUtils::validationLayers.size());
        createInfo.ppEnabledLayerNames = VulkanUtils::validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pNext = nullptr;
    }

    QI_CORE_INFO("Creating Vulkan instance");

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance!");

    QI_CORE_INFO("Vulkan instance created successfully");

}

VkInstance VulkanInstance::getVkInstance() {
    return m_instance;
}

std::vector<const char*> VulkanInstance::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char* const* glfwExtensions = VulkanPlatform::getRequiredVulkanExtensions(glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if constexpr (VulkanUtils::enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanInstance::setupDebugMessenger() {
    if constexpr (VulkanUtils::enableValidationLayers) {
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        populateDebugMessengerCreateInfo(debugCreateInfo);

        VkResult debugResult = createDebugUtilsMessengerEXT(
            m_instance,
            &debugCreateInfo,
            nullptr,
            &m_debugMessenger
        );

        QI_RENDERER_ASSERT(debugResult == VK_SUCCESS, "Failed to set up Vulkan debug messenger!");
    }
}

}
