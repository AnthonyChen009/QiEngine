#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "types/PushConstants.hpp"
#include "renderer/utils/VulkanUtils.hpp"

namespace Qi {

class VulkanGraphicsPipeline {
public:
    VulkanGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VulkanUtils::PipelineType type);
    ~VulkanGraphicsPipeline();

    VkPipeline getPipeline() const;
    VkPipelineLayout getPipelineLayout() const;
    VkDescriptorSetLayout getDescriptorSetLayout() const;

private:
    void createDescriptorSetLayout();
    void createGraphicsPipeline(VkRenderPass renderPass);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    VulkanUtils::PipelineType m_type;
    VkDevice m_device = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
};

}
