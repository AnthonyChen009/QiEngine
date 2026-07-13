#pragma once

#include "core/Window.hpp"
#include "glm/glm.hpp"
#include "vulkan/Texture2D.hpp"
#include "Renderer.hpp"
namespace Qi {

class Renderer2D {
public:
    Renderer2D(Renderer& render);
    void drawQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color);
    void drawTexturedQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color, Texture2D* texture);
    std::shared_ptr<Texture2D> loadTexture(const std::string& path);
    void beginScene();
    void endScene();

private:
    Renderer& m_renderer;

};

}
