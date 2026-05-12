#include "Renderer2D.hpp"
#include "renderer/vulkan/VulkanRenderer.hpp"
#include "core/Base.hpp"
#include "core/Assert.hpp"
#include "Renderer.hpp"
#include "types/PushConstants.hpp"

namespace Qi {

Renderer2D::Renderer2D() {

}

Scope<Renderer2D> Renderer2D::s_instance = nullptr;

void Renderer2D::init(Window& window, GraphicsAPI api) {
    Renderer::init(window, api);
    s_instance = createScope<Renderer2D>();
}

bool Renderer2D::beginFrame() {
    return Renderer::beginFrame();
}

void Renderer2D::endFrame() {
    Renderer::endFrame();
}

void Renderer2D::drawQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color) {
    PushConstant2D push{};
    push.transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
    push.color = color;
    push.textureIndex = 0xFFFFFFFF;
    Renderer::getBackend()->pushConstants(push);
    Renderer::getBackend()->drawIndexed(6);
}

void Renderer2D::updateUniformBuffer() {
    Renderer::getBackend()->updateUniformBuffer();
}

void Renderer2D::bindPipeline() {
    Renderer::getBackend()->bindPipeline();
}

void Renderer2D::onWindowResize(uint32_t width, uint32_t height) {
    Renderer::onWindowResize(width, height);
}

void Renderer2D::drawTexturedQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color, Texture2D* texture) {

    PushConstant2D push{};
    push.transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
                    * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f))
                    * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
    push.color = color;
    push.textureIndex = texture ? texture->getIndex() : 0;
    Renderer::getBackend()->pushConstants(push);
    Renderer::getBackend()->drawIndexed(6);
}

Texture2D* Renderer2D::loadTexture(const std::string& path) {
    return Renderer::getBackend()->getOrLoadTexture(path);
}

void Renderer2D::shutdown() {
    Renderer::shutdown();
    s_instance = nullptr;
}

}
