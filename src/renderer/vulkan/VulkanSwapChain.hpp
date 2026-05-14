#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "core/Window.hpp"
#include "renderer/types/SwapChainSupportDetails.hpp"
#include "renderer/types/QueueFamilyIndices.hpp"

namespace Qi {

class VulkanSwapChain {
public:
    VulkanSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, Window& window, const QueueFamilyIndices& indices);
    ~VulkanSwapChain();

    VkSwapchainKHR getSwapChain() const;
    VkFormat getImageFormat() const;
    VkExtent2D getExtent() const;
    const std::vector<VkImage>& getImages() const;
    const std::vector<VkImageView>& getImageViews() const;
    void recreate(Window& window);
    void setVSync(bool isVSync);
private:
    void createSwapChain(Window& window, VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE);
    void createImageViews();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window& window);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    QueueFamilyIndices m_queueFamilyIndices;
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_swapChainExtent = {0, 0};
    std::vector<VkImageView> m_swapChainImageViews;
    bool m_isVSync = false;
};

}
