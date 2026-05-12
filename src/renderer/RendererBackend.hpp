#pragma once
#include "core/Window.hpp"
#include "types/PushConstants.hpp"
#include "renderer/vulkan/VulkanTexture.hpp"

namespace Qi {

class RendererBackend {
public:
    virtual ~RendererBackend() = default;

    virtual void init(Window& window) = 0;
    virtual void shutdown() = 0;

    virtual bool beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void bindPipeline() = 0;
    virtual void drawIndexed(uint32_t count) = 0;
    virtual void onWindowResize(uint32_t width, uint32_t height) = 0;
    virtual void updateUniformBuffer() = 0;
    virtual void pushConstants(const PushConstant2D& push) = 0;
    virtual Texture2D* getOrLoadTexture(const std::string& path) = 0;
    virtual void initImGui(Window* window) = 0;
    virtual void shutdownImGui() = 0;
    virtual void beginImGuiFrame() = 0;
    virtual void renderImGui() = 0;
};

}
