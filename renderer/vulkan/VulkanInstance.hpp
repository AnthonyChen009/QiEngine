#pragma once

#include <string>
#include <vulkan/vulkan.h>
namespace Qi {

class VulkanInstance {
public:
    VulkanInstance();
    ~VulkanInstance();
    VkInstance getVkInstance();
    void createVkInstance(const std::string& appName);


private:
    void setupDebugMessenger();
    std::vector<const char*> getRequiredExtensions();

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
};

}
