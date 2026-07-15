#include "Renderer3D.hpp"
#include "core/Log.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/vector_float4.hpp>
namespace Qi {

Renderer3D::Renderer3D(Renderer& renderer) : m_renderer(renderer) {

}

void Renderer3D::beginScene() {
    m_renderer.getBackend()->bindPipeline(VulkanUtils::PipelineType::Pipeline3D);
}

void Renderer3D::drawCube() {
    // m_renderer.getBackend()->bindBuffers(*m_renderer.getBackend()->m_vertexBuffer, *m_renderer.getBackend()->m_indexBuffer);
    // PushConstant push{};
    // push.transform = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // push.color = glm::vec4(1.0);
    // push.textureIndex = 0xFFFFFFFF;
    // m_renderer.getBackend()->pushConstants3D(push);
    // m_renderer.getBackend()->drawIndexed(6);
}

void Renderer3D::drawMesh(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform) {
    m_renderer.getBackend()->bindBuffers(mesh->getVertexBuffer(), mesh->getIndexBuffer());

    PushConstant push{};
    push.transform = transform;
    push.color = glm::vec4(1.0);
    push.textureIndex = 0xFFFFFFFF;

    m_renderer.getBackend()->pushConstants3D(push);
    m_renderer.getBackend()->drawIndexed(1);

}

void Renderer3D::endScene() {

}

}
