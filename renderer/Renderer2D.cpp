#include "Renderer2D.hpp"
#include "vulkan/VulkanRenderer.hpp"
#include "opengl/OpenGLRenderer.hpp"
#include "Base.hpp"
#include "Assert.hpp"
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

void Renderer2D::shutdown() {
    Renderer::shutdown();
    s_instance = nullptr;
}

}
