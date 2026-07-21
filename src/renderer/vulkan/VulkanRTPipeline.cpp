// VulkanRTPipeline.cpp
#include "VulkanRTPipeline.hpp"
#include "core/Assert.hpp"
#include "core/FileSystem.hpp"
#include "VulkanCommands.hpp"

namespace Qi {

namespace {
    PFN_vkCreateRayTracingPipelinesKHR pfnCreateRTPipelines = nullptr;
    PFN_vkGetRayTracingShaderGroupHandlesKHR pfnGetShaderGroupHandles = nullptr;
    PFN_vkCmdTraceRaysKHR pfnCmdTraceRays = nullptr;

    void loadRTPipelineFunctionsIfNeeded(VkDevice device) {
        if (pfnCreateRTPipelines) return;
        pfnCreateRTPipelines = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(
            vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR"));
        pfnGetShaderGroupHandles = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(
            vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR"));
        pfnCmdTraceRays = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(
            vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR"));
        QI_RENDERER_ASSERT(pfnCreateRTPipelines && pfnGetShaderGroupHandles && pfnCmdTraceRays,
                            "Failed to load ray tracing pipeline function pointers!");
    }
}

VulkanRTPipeline::VulkanRTPipeline(VkDevice device) : m_device(device) {
    loadRTPipelineFunctionsIfNeeded(device);
    createDescriptorSetLayout();
    createRTPipeline();
}

VulkanRTPipeline::~VulkanRTPipeline() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
    if (m_descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
        m_descriptorSetLayout = VK_NULL_HANDLE;
    }
}

VkPipeline VulkanRTPipeline::getPipeline() const { return m_pipeline; }
VkPipelineLayout VulkanRTPipeline::getPipelineLayout() const { return m_pipelineLayout; }
VkDescriptorSetLayout VulkanRTPipeline::getDescriptorSetLayout() const { return m_descriptorSetLayout; }

void VulkanRTPipeline::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding tlasBinding{};
    tlasBinding.binding = 0;
    tlasBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    tlasBinding.descriptorCount = 1;
    tlasBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

    VkDescriptorSetLayoutBinding outputImageBinding{};
    outputImageBinding.binding = 1;
    outputImageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    outputImageBinding.descriptorCount = 1;
    outputImageBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

    VkDescriptorSetLayoutBinding cameraUboBinding{};
    cameraUboBinding.binding = 2;
    cameraUboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraUboBinding.descriptorCount = 1;
    cameraUboBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = { tlasBinding, outputImageBinding, cameraUboBinding };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkResult result = vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create RT descriptor set layout!");
}

void VulkanRTPipeline::buildSBT(VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue) {
    // Query handle size/alignment — these are device-specific, not something to hardcode.
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProperties{};
    rtProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 deviceProperties2{};
    deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    deviceProperties2.pNext = &rtProperties;
    vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProperties2);

    const uint32_t handleSize = rtProperties.shaderGroupHandleSize;
    const uint32_t handleAlignment = rtProperties.shaderGroupHandleAlignment;
    const uint32_t baseAlignment = rtProperties.shaderGroupBaseAlignment;

    auto alignUp = [](uint32_t value, uint32_t alignment) {
        return (value + alignment - 1) & ~(alignment - 1);
    };

    const uint32_t handleSizeAligned = alignUp(handleSize, handleAlignment);
    const uint32_t groupCount = static_cast<uint32_t>(m_shaderGroups.size()); // 3: raygen, miss, hit

    // Fetch the raw shader group handles from the pipeline.
    std::vector<uint8_t> handleData(groupCount * handleSize);
    VkResult result = pfnGetShaderGroupHandles(
        m_device, m_pipeline, 0, groupCount,
        handleData.size(), handleData.data()
    );
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to get shader group handles!");

    // Each region (raygen/miss/hit) must start at an address aligned to baseAlignment.
    // Raygen region is special-cased in the spec: its size must equal its stride
    // (exactly one entry, no array).
    const uint32_t raygenRegionSize = alignUp(handleSizeAligned, baseAlignment);
    const uint32_t missRegionSize = alignUp(handleSizeAligned, baseAlignment);
    const uint32_t hitRegionSize = alignUp(handleSizeAligned, baseAlignment);

    const VkDeviceSize sbtSize = raygenRegionSize + missRegionSize + hitRegionSize;

    // Build the SBT contents on the CPU first, laid out with correct padding.
    std::vector<uint8_t> sbtData(sbtSize, 0);
    // Group 0 = raygen
    memcpy(sbtData.data(), handleData.data() + 0 * handleSize, handleSize);
    // Group 1 = miss
    memcpy(sbtData.data() + raygenRegionSize, handleData.data() + 1 * handleSize, handleSize);
    // Group 2 = hit
    memcpy(sbtData.data() + raygenRegionSize + missRegionSize, handleData.data() + 2 * handleSize, handleSize);

    VulkanBuffer stagingBuffer(m_device, physicalDevice);
    stagingBuffer.create(sbtSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.setData(sbtData.data(), sbtSize);

    m_sbtBuffer = std::make_unique<VulkanBuffer>(m_device, physicalDevice);
    m_sbtBuffer->create(sbtSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VulkanCommands::copyBuffer(m_device, commandPool, queue, stagingBuffer.getBuffer(), m_sbtBuffer->getBuffer(), sbtSize);

    VkDeviceAddress sbtAddress = m_sbtBuffer->getDeviceAddress();

    m_raygenRegion.deviceAddress = sbtAddress;
    m_raygenRegion.stride = raygenRegionSize;
    m_raygenRegion.size = raygenRegionSize;

    m_missRegion.deviceAddress = sbtAddress + raygenRegionSize;
    m_missRegion.stride = handleSizeAligned;
    m_missRegion.size = missRegionSize;

    m_hitRegion.deviceAddress = sbtAddress + raygenRegionSize + missRegionSize;
    m_hitRegion.stride = handleSizeAligned;
    m_hitRegion.size = hitRegionSize;
}

void VulkanRTPipeline::cmdTraceRays(VkCommandBuffer commandBuffer, uint32_t width, uint32_t height) {
    VkStridedDeviceAddressRegionKHR raygenRegion = m_raygenRegion;
    VkStridedDeviceAddressRegionKHR missRegion = m_missRegion;
    VkStridedDeviceAddressRegionKHR hitRegion = m_hitRegion;
    VkStridedDeviceAddressRegionKHR callableRegion{}; // unused, must still be passed zeroed

    pfnCmdTraceRays(commandBuffer, &raygenRegion, &missRegion, &hitRegion, &callableRegion, width, height, 1);
}

void VulkanRTPipeline::createRTPipeline() {
    // Shader stages: raygen, miss, closest-hit.
    auto raygenCode = FileSystem::readBinaryFile("shaders/RT/raygen.spv");
    auto missCode = FileSystem::readBinaryFile("shaders/RT/miss.spv");
    auto chitCode = FileSystem::readBinaryFile("shaders/RT/closestHit.spv");

    VkShaderModule raygenModule = createShaderModule(raygenCode);
    VkShaderModule missModule = createShaderModule(missCode);
    VkShaderModule chitModule = createShaderModule(chitCode);

    std::array<VkPipelineShaderStageCreateInfo, 3> stages{};
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    stages[0].module = raygenModule;
    stages[0].pName = "main";

    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_MISS_BIT_KHR;
    stages[1].module = missModule;
    stages[1].pName = "main";

    stages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[2].stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    stages[2].module = chitModule;
    stages[2].pName = "main";

    m_shaderGroups.clear();

    VkRayTracingShaderGroupCreateInfoKHR raygenGroup{};
    raygenGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    raygenGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    raygenGroup.generalShader = 0; // index into stages[]
    raygenGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
    raygenGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
    raygenGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
    m_shaderGroups.push_back(raygenGroup);

    VkRayTracingShaderGroupCreateInfoKHR missGroup{};
    missGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    missGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    missGroup.generalShader = 1;
    missGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
    missGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
    missGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
    m_shaderGroups.push_back(missGroup);

    VkRayTracingShaderGroupCreateInfoKHR hitGroup{};
    hitGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    hitGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    hitGroup.generalShader = VK_SHADER_UNUSED_KHR;
    hitGroup.closestHitShader = 2;
    hitGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
    hitGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
    m_shaderGroups.push_back(hitGroup);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;


    VkResult layoutResult = vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    QI_RENDERER_ASSERT(layoutResult == VK_SUCCESS, "Failed to create RT pipeline layout!");

    VkRayTracingPipelineCreateInfoKHR pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
    pipelineInfo.pStages = stages.data();
    pipelineInfo.groupCount = static_cast<uint32_t>(m_shaderGroups.size());
    pipelineInfo.pGroups = m_shaderGroups.data();
    pipelineInfo.maxPipelineRayRecursionDepth = 1; // no secondary rays yet
    pipelineInfo.layout = m_pipelineLayout;

    VkResult result = pfnCreateRTPipelines(
        m_device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline
    );
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create ray tracing pipeline!");

    vkDestroyShaderModule(m_device, chitModule, nullptr);
    vkDestroyShaderModule(m_device, missModule, nullptr);
    vkDestroyShaderModule(m_device, raygenModule, nullptr);
}

VkShaderModule VulkanRTPipeline::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create shader module!");
    return shaderModule;
}

}
