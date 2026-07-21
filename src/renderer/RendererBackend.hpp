#pragma once
#include "core/Window.hpp"
#include "renderer/types/RTInstanceData.hpp"
#include "renderer/types/SkyUbo.hpp"
#include "renderer/types/UniformBufferObject.hpp"
#include "renderer/vulkan/VulkanAccelerationStructure.hpp"
#include "renderer/vulkan/VulkanDevice.hpp"
#include "renderer/vulkan/VulkanIndexBuffer.hpp"
#include "renderer/vulkan/VulkanVertexBuffer.hpp"
#include "types/PushConstants.hpp"
#include "renderer/vulkan/VulkanTexture.hpp"
#include "utils/VulkanUtils.hpp"

namespace Qi {

class RendererBackend {
public:
    virtual ~RendererBackend() = default;

    virtual void init(Window& window) = 0;
    virtual void shutdown() = 0;

    virtual bool beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void bindPipeline(VulkanUtils::PipelineType type) = 0;
    virtual void bindBuffers(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer) = 0;
    virtual void drawIndexed(uint32_t count) = 0;
    virtual void onWindowResize(uint32_t width, uint32_t height) = 0;
    virtual void onVysncToggle(bool isVSync) {};
    virtual void updateUniformBuffer2D() = 0;
    virtual void updateUniformBuffer3D(UniformBufferObject& ubo) = 0;
    virtual void updateUniformBufferSky(SkyUniformBufferObject& ubo) = 0;
    virtual void pushConstants2D(const PushConstant& push) = 0;
    virtual void pushConstants3D(const PushConstant& push) = 0;
    virtual void drawFullscreenTriangle() = 0;
    virtual std::shared_ptr<Texture2D> getOrLoadTexture(const std::string& path) = 0;
    virtual std::shared_ptr<VertexBuffer> createVertexBuffer(const std::vector<Vertex>& vertices) = 0;
    virtual std::shared_ptr<IndexBuffer> createIndexBuffer(const std::vector<uint32_t>& indices) = 0;


    virtual void initImGui(Window* window) = 0;
    virtual void shutdownImGui() = 0;
    virtual void beginImGuiFrame() = 0;
    virtual void renderImGui() = 0;

    virtual bool hasRTSupport() = 0;
    virtual std::unique_ptr<VulkanAccelerationStructure> createBLAS(
        const VertexBuffer& vertexBuffer, uint32_t vertexCount, size_t vertexStride,
        const IndexBuffer& indexBuffer, uint32_t indexCount,
        bool allowUpdate = false) = 0;
    virtual void updateTLAS(const std::vector<RTInstanceData>& instances) = 0;
};

}
