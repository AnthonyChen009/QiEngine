#pragma once

#include "Window.hpp"
#include "RendererBackend.hpp"

namespace Qi {

class Renderer {
public:
    Renderer(GraphicsAPI api);

    static void init(Window& window, GraphicsAPI api);
    static void shutdown();

    static bool beginFrame();
    static void endFrame();
    static void drawQuad();
    static void onWindowResize(uint32_t width, uint32_t height);

    static void drawIndexed();

private:
    static Scope<Renderer> s_instance;
    Scope<RendererBackend> m_backend;
};

}
