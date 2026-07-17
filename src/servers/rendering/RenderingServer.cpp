#include "RenderingServer.hpp"
#include "core/Base.hpp"
#include "core/Log.hpp"

#include "renderer/IndexBuffer.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Renderer2D.hpp"
#include "renderer/Renderer3D.hpp"
#include "renderer/VertexBuffer.hpp"
#include "renderer/types/UniformBufferObject.hpp"
#include "scene/Components.hpp"
#include "servers/rendering/ImGuiLayer.hpp"
#include "servers/rendering/PrimitiveMeshLibrary.hpp"
#include <memory>

namespace Qi {

RenderingServer::RenderingServer(Window& window, GraphicsAPI graphicsAPI) {
    m_renderer = createScope<Renderer>(graphicsAPI);
    m_renderer->init(window, graphicsAPI);

    m_renderer2D = createScope<Renderer2D>(*m_renderer, m_primitives.getQuad());
    m_renderer3D = createScope<Renderer3D>(*m_renderer);

    m_imGuiLayer = createScope<ImGuiLayer>(window, *m_renderer);
    m_imGuiLayer->init();
}

RenderingServer::~RenderingServer() {
    m_imGuiLayer->shutdown();
}

bool RenderingServer::beginFrame() {
    return m_renderer->beginFrame();
}

void RenderingServer::render(Scene& scene) {
    //camera stuff
    render3D(scene);
    render2D(scene);

}

void RenderingServer::render2D(Scene& scene) {
    m_renderer2D->beginScene();
    auto view = scene.getRegistry().view<TransformComponent, SpriteComponent>();
    for (auto entity : view) {
        TransformComponent& transform = view.get<TransformComponent>(entity);
        SpriteComponent& sprite = view.get<SpriteComponent>(entity);
        if (sprite.texture)
            m_renderer2D->drawTexturedQuad(transform.worldPosition, transform.size, transform.worldRotation, sprite.color, sprite.texture);
        else
            m_renderer2D->drawQuad(transform.worldPosition, transform.size, transform.worldRotation, sprite.color);
    }
    m_renderer2D->endScene();
}

void RenderingServer::render3D(Scene& scene) {
    Camera3D* camera = scene.getActiveCamera();
    if (!camera) {
        if (!m_warnedNoCamera) {
            QI_WARN("No active 3D camera found. Skipping 3D rendering.");
            m_warnedNoCamera = true;
        }
        return;
    }

    m_warnedNoCamera = false;

    m_renderer3D->beginScene();

    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(camera->getViewMatrix()));
    glm::mat4 skyProj = camera->getProjectionMatrix();
    skyProj[1][1] *= -1;

    SkyUniformBufferObject skyUbo{};
    skyUbo.invViewProj = glm::inverse(skyProj * viewNoTranslation);
    //move into m_renderer later
    m_renderer->getBackend()->updateUniformBufferSky(skyUbo);
    m_renderer->getBackend()->drawFullscreenTriangle();

    UniformBufferObject ubo{};
    ubo.view = camera->getViewMatrix();
    ubo.proj = camera->getProjectionMatrix();
    ubo.proj[1][1] *= -1;

    auto lightView = scene.getRegistry().view<DirectionalLightComponent>();
    if (!lightView.empty()) {
        auto& light = lightView.get<DirectionalLightComponent>(lightView.front());
        ubo.lightDirection = glm::normalize(light.direction);
        ubo.lightColor = light.color;
        ubo.lightIntensity = light.intensity;
    }

    ubo.ambientColor = glm::vec3(1.0f);
    ubo.ambientIntensity = 0.1f;

    m_renderer->updateUniformBuffer3D(ubo);

    auto view = scene.getRegistry().view<Transform3DComponent, MeshComponent>();
    for (auto entity : view) {
        Transform3DComponent& transform = view.get<Transform3DComponent>(entity);
        MeshComponent& meshComponent = view.get<MeshComponent>(entity);
        if (meshComponent.mesh) {
            m_renderer3D->drawMesh(meshComponent.mesh, transform.worldTransform, meshComponent.albedoTexture);
        }
    }

    m_renderer3D->endScene();
}


std::shared_ptr<Mesh> RenderingServer::createMesh(const std::string& path) {
    return nullptr;
}
//use by meshlib only
std::shared_ptr<Mesh> RenderingServer::createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    if (vertices.empty() || indices.empty()) {
        QI_CORE_ERROR("createMesh called with empty vertex/index data — skipping mesh creation.");
        return nullptr;
    }
    std::shared_ptr<VertexBuffer> vertexBuffer = m_renderer->getBackend()->createVertexBuffer(vertices);
    std::shared_ptr<IndexBuffer> indexBuffer = m_renderer->getBackend()->createIndexBuffer(indices);
    return std::make_shared<Mesh>(vertexBuffer, indexBuffer);
}

void RenderingServer::endFrame() {
    m_renderer->endFrame();
}

void RenderingServer::setVSync(bool enabled) {
    m_renderer->setVSync(enabled);
}

void RenderingServer::onWindowResize(unsigned int x, unsigned int y) {
    m_renderer->onWindowResize(x, y);
}

void RenderingServer::onEvent(Qi::Event& event) {
    m_imGuiLayer->onEvent(event);
}

std::shared_ptr<Texture2D> RenderingServer::createTexture2D(const std::string& path) {
    return m_renderer2D->loadTexture(path);
}

void RenderingServer::beginImGui() {
    m_imGuiLayer->begin();
}

void RenderingServer::endImGui() {
    m_imGuiLayer->end();
}

PrimitiveMeshLibrary& RenderingServer::getPrimitives() {
    return m_primitives;
}

ImGuiLayer& RenderingServer::getImGuiLayer() {
    return *m_imGuiLayer;
}


}
