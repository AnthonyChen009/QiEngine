#include "Renderer3D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/vector_float4.hpp>
namespace Qi {

Renderer3D::Renderer3D(Renderer& renderer) : m_renderer(renderer) {

}

void Renderer3D::beginScene() {
    m_renderer.updateUniformBuffer3D();
    m_renderer.getBackend()->bindPipeline(VulkanUtils::PipelineType::Pipeline3D);
}

void Renderer3D::drawCube() {
    PushConstant push{};
    push.transform = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    push.color = glm::vec4(1.0);
    push.textureIndex = 0xFFFFFFFF;
    m_renderer.getBackend()->pushConstants3D(push);
    m_renderer.getBackend()->drawIndexed(6);
}

void Renderer3D::endScene() {

}

}
