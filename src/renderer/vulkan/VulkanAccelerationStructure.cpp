#include "VulkanAccelerationStructure.hpp"
#include "renderer/vulkan/VulkanCommands.hpp"
#include "core/Assert.hpp"

namespace Qi {

namespace {
    PFN_vkGetAccelerationStructureBuildSizesKHR pfnGetBuildSizes = nullptr;
    PFN_vkCreateAccelerationStructureKHR pfnCreateAS = nullptr;
    PFN_vkDestroyAccelerationStructureKHR pfnDestroyAS = nullptr;
    PFN_vkCmdBuildAccelerationStructuresKHR pfnCmdBuildAS = nullptr;
    PFN_vkGetAccelerationStructureDeviceAddressKHR pfnGetASDeviceAddress = nullptr;

    void loadASFunctionsIfNeeded(VkDevice device) {
        if (pfnGetBuildSizes) return;
        pfnGetBuildSizes = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR"));
        pfnCreateAS = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
        pfnDestroyAS = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR"));
        pfnCmdBuildAS = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR"));
        pfnGetASDeviceAddress = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR"));

        QI_RENDERER_ASSERT(pfnGetBuildSizes && pfnCreateAS && pfnDestroyAS && pfnCmdBuildAS && pfnGetASDeviceAddress, "Failed to load acceleration structure function pointers!");
    }
}

VulkanAccelerationStructure::VulkanAccelerationStructure(VkDevice device, VkPhysicalDevice physicalDevice) : m_device(device), m_physicalDevice(physicalDevice), m_asBuffer(device, physicalDevice) {
    loadASFunctionsIfNeeded(device);
}

void VulkanAccelerationStructure::buildBLAS(VkCommandPool commandPool, VkQueue queue, const VulkanBuffer& vertexBuffer, uint32_t vertexCount, VkDeviceSize vertexStride, const VulkanBuffer& indexBuffer, uint32_t indexCount, bool allowUpdate) {
    m_allowsUpdate = allowUpdate;
    VkAccelerationStructureGeometryTrianglesDataKHR triangles{};
    triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    triangles.vertexData.deviceAddress = vertexBuffer.getDeviceAddress();
    triangles.vertexStride = vertexStride;
    triangles.maxVertex = vertexCount - 1;
    triangles.indexType = VK_INDEX_TYPE_UINT32;
    //may need submesh ind offset (indexAddr + submesh.indexOffset * sizeof(uint32_t))
    triangles.indexData.deviceAddress = indexBuffer.getDeviceAddress();

    VkAccelerationStructureGeometryKHR geometry{};
    geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    geometry.geometry.triangles = triangles;
    geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

    VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
    buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    buildInfo.flags = allowUpdate ? (VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR) : VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.geometryCount = 1;
    buildInfo.pGeometries = &geometry;

    const uint32_t primitiveCount = indexCount / 3;

    VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
    sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    pfnGetBuildSizes(m_device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo, &primitiveCount, &sizeInfo);

    m_asBuffer.create(sizeInfo.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkAccelerationStructureCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    createInfo.buffer = m_asBuffer.getBuffer();
    createInfo.size = sizeInfo.accelerationStructureSize;
    createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    VkResult result = pfnCreateAS(m_device, &createInfo, nullptr, &m_accelerationStructure);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create acceleration structure!");

    buildInfo.dstAccelerationStructure = m_accelerationStructure;

    VulkanBuffer scratchBuffer(m_device, m_physicalDevice);
    scratchBuffer.create(sizeInfo.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    buildInfo.scratchData.deviceAddress = scratchBuffer.getDeviceAddress();

    VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
    rangeInfo.primitiveCount = primitiveCount;
    const VkAccelerationStructureBuildRangeInfoKHR* pRangeInfo = &rangeInfo;

    VkCommandBuffer cmd = VulkanCommands::beginSingleTimeCommands(m_device, commandPool);
    pfnCmdBuildAS(cmd, 1, &buildInfo, &pRangeInfo);
    VulkanCommands::endSingleTimeCommands(m_device, commandPool, queue, cmd);

    VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = m_accelerationStructure;
    m_deviceAddress = pfnGetASDeviceAddress(m_device, &addressInfo);
}

void VulkanAccelerationStructure::buildTLAS(VkCommandBuffer commandBuffer, const VulkanBuffer& instanceBuffer, VulkanBuffer& scratchBuffer, uint32_t instanceCount, bool allowUpdate) {
    m_allowsUpdate = allowUpdate;

    VkAccelerationStructureGeometryInstancesDataKHR instancesData{};
    instancesData.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
    instancesData.arrayOfPointers = VK_FALSE;
    instancesData.data.deviceAddress = instanceBuffer.getDeviceAddress();

    VkAccelerationStructureGeometryKHR geometry{};
    geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    geometry.geometry.instances = instancesData;

    VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
    buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    buildInfo.flags = allowUpdate ? (VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR) : VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.geometryCount = 1;
    buildInfo.pGeometries = &geometry;

    VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
    sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    pfnGetBuildSizes(m_device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo, &instanceCount, &sizeInfo);

    m_asBuffer.create(
        sizeInfo.accelerationStructureSize,
        VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    VkAccelerationStructureCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    createInfo.buffer = m_asBuffer.getBuffer();
    createInfo.size = sizeInfo.accelerationStructureSize;
    createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

    VkResult result = pfnCreateAS(m_device, &createInfo, nullptr, &m_accelerationStructure);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create TLAS!");

    buildInfo.dstAccelerationStructure = m_accelerationStructure;

    scratchBuffer.create(
        sizeInfo.buildScratchSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    buildInfo.scratchData.deviceAddress = scratchBuffer.getDeviceAddress();

    VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
    rangeInfo.primitiveCount = instanceCount;
    const VkAccelerationStructureBuildRangeInfoKHR* pRangeInfo = &rangeInfo;

    pfnCmdBuildAS(commandBuffer, 1, &buildInfo, &pRangeInfo);

    VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = m_accelerationStructure;
    m_deviceAddress = pfnGetASDeviceAddress(m_device, &addressInfo);
}

VulkanAccelerationStructure::~VulkanAccelerationStructure() {
    destroy();
}

void VulkanAccelerationStructure::destroy() {
    if (m_accelerationStructure != VK_NULL_HANDLE) {
        pfnDestroyAS(m_device, m_accelerationStructure, nullptr);
        m_accelerationStructure = VK_NULL_HANDLE;
    }
    m_asBuffer.destroy();
}

}
