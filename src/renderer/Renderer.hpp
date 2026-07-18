#pragma once

#include "core/Window.hpp"
#include "RendererBackend.hpp"

namespace Qi {

class Renderer {
public:
    Renderer(GraphicsAPI api);

    void init(Window& window, GraphicsAPI api);
    void shutdown();

    bool beginFrame();
    void endFrame();
    void drawQuad();
    void onWindowResize(uint32_t width, uint32_t height);
    void updateUniformBuffer2D();
    void updateUniformBuffer3D(UniformBufferObject& ubo);
    void bindPipeline();
    void setVSync(bool isVSync);
    void drawIndexed();
    RendererBackend* getBackend();

private:
    Scope<RendererBackend> m_backend;
};

}
