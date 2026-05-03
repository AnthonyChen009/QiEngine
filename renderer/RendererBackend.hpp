#pragma once
#include "Window.hpp"

namespace Qi {

class RendererBackend {
public:
    virtual ~RendererBackend() = default;

    virtual void init(Window& window) = 0;
    virtual void shutdown() = 0;

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void onWindowResize(uint32_t width, uint32_t height) = 0;

};

}
