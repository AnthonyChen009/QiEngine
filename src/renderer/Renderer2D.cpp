#include "Renderer2D.hpp"
#include "renderer/vulkan/VulkanRenderer.hpp"
#include "core/Base.hpp"
#include "core/Assert.hpp"
#include "Renderer.hpp"
#include "types/PushConstants.hpp"


namespace Qi {

Renderer2D::Renderer2D(Renderer& renderer, std::shared_ptr<Mesh> quad) : m_renderer(renderer), m_quad(quad) {

}

void Renderer2D::drawQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color) {
    m_renderer.getBackend()->bindBuffers(m_quad->getVertexBuffer(), m_quad->getIndexBuffer());
    PushConstant push{};
    push.transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
    push.color = color;
    push.textureIndex = 0xFFFFFFFF;
    m_renderer.getBackend()->pushConstants2D(push);
    m_renderer.getBackend()->drawIndexed(6);
}

void Renderer2D::beginScene() {
    m_renderer.updateUniformBuffer2D();
    m_renderer.getBackend()->bindPipeline(VulkanUtils::PipelineType::Pipeline2D);
}

void Renderer2D::endScene() {

}

void Renderer2D::drawTexturedQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color, Texture2D* texture) {
    m_renderer.getBackend()->bindBuffers(m_quad->getVertexBuffer(), m_quad->getIndexBuffer());
    PushConstant push{};
    push.transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
                    * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f))
                    * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
    push.color = color;
    push.textureIndex = texture ? texture->getIndex() : 0;
    m_renderer.getBackend()->pushConstants2D(push);
    m_renderer.getBackend()->drawIndexed(6);
}

std::shared_ptr<Texture2D> Renderer2D::loadTexture(const std::string& path) {
    return m_renderer.getBackend()->getOrLoadTexture(path);
}

}
