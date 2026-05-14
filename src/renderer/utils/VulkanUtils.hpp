#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "renderer/types/SwapChainSupportDetails.hpp"

namespace Qi::VulkanUtils {
enum class PipelineType { Pipeline2D, Pipeline3D };

static constexpr bool enableValidationLayers =
#ifdef QI_DEBUG
    true;
#else
    false;
#endif

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

}
