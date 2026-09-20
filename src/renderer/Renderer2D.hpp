#pragma once

#include "core/Window.hpp"
#include "glm/glm.hpp"
#include "vulkan/Texture2D.hpp"
#include "Renderer.hpp"
#include "renderer/vulkan/Mesh.hpp"


namespace Qi {

class Renderer2D {
public:
    Renderer2D(Renderer& renderer, std::shared_ptr<Mesh> quad);
    void drawQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color);
    void drawTexturedQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color, std::shared_ptr<Texture2D>& texture);
    std::shared_ptr<Texture2D> loadTexture(const std::string& path);
    void beginScene();
    void endScene();

private:
    Renderer& m_renderer;
    //for 2d quad
    std::shared_ptr<Mesh> m_quad;
};

}
