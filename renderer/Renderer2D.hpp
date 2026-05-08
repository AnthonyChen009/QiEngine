#pragma once

#include "Window.hpp"
#include "glm/glm.hpp"

namespace Qi {

class Renderer2D {
public:
    Renderer2D();

    static void init(Window& window, GraphicsAPI api);
    static void shutdown();

    static bool beginFrame();
    static void endFrame();
    static void drawQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color);
    static void onWindowResize(uint32_t width, uint32_t height);
    static void updateUniformBuffer();
    static void bindPipeline();

private:
    static Scope<Renderer2D> s_instance;

};

}
