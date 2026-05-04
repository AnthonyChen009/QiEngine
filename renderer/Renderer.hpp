#pragma once

#include "Window.hpp"
#include "RendererBackend.hpp"

namespace Qi {

class Renderer {
public:
    Renderer(GraphicsAPI api);

    void init(Window& window);
    void shutdown();

    bool beginFrame();
    void endFrame();
    void bindPipeline();
    void onWindowResize(uint32_t width, uint32_t height);

    void draw(uint32_t vertexCount);

private:
    Scope<RendererBackend> m_backend;
};

}
