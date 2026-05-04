#pragma once
#include "Window.hpp"

namespace Qi {

class RendererBackend {
public:
    virtual ~RendererBackend() = default;

    virtual void init(Window& window) = 0;
    virtual void shutdown() = 0;

    virtual bool beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void bindPipeline() = 0;
    virtual void draw(uint32_t vertexCount) = 0;
    virtual void onWindowResize(uint32_t width, uint32_t height) = 0;

};

}
