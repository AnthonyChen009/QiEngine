#pragma once

#include "core/Window.hpp"
#include "glm/glm.hpp"
#include "vulkan/Texture2D.hpp"
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
    static void drawTexturedQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color, Texture2D* texture);
    static Texture2D* loadTexture(const std::string& path);
private:
    static Scope<Renderer2D> s_instance;

};

}
