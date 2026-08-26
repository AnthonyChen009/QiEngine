#include "RenderingServer.hpp"
#include "core/Assert.hpp"
#include "core/Base.hpp"
#include "core/Log.hpp"

#include "events/RenderingEvents.hpp"
#include "renderer/IndexBuffer.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Renderer2D.hpp"
#include "renderer/Renderer3D.hpp"
#include "renderer/VertexBuffer.hpp"
#include "renderer/types/RTCameraUBO.hpp"
#include "renderer/types/UniformBufferObject.hpp"
#include "renderer/vulkan/VulkanAccelerationStructure.hpp"
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
    m_renderer->getBackend()->shutdown();
}

bool RenderingServer::beginFrame() {
    return m_renderer->beginFrame();
}

void RenderingServer::render(Scene& scene) {
    //camera stuff
    render3D(scene);
    render2D(scene);

}

void RenderingServer::waitIdle() {
    m_renderer->getBackend()->waitIdle();
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

    m_renderer->getBackend()->uploadMaterialsIfDirty();

    UniformBufferObject ubo{};
    ubo.view = camera->getViewMatrix();
    ubo.proj = camera->getProjectionMatrix();
    ubo.proj[1][1] *= -1;
    //fix accumulation buffer when instance size changes or an object has moved
    if (m_renderer->getBackend()->hasRTSupport() && m_useHybridRT) {
        bool needsUpdate = false;
        std::vector<RTInstanceData> instances;
        auto rtView = scene.getRegistry().view<Transform3DComponent, MeshComponent>();

        uint32_t instanceIndex = 0;
        for (auto entity : rtView) {
            auto& meshComp = rtView.get<MeshComponent>(entity);
            auto& transformComp = rtView.get<Transform3DComponent>(entity);
            if (!meshComp.mesh || !meshComp.mesh->hasBLAS()) continue;
            RTInstanceData instance;
            instance.blasAddress = meshComp.mesh->getBLAS()->getDeviceAddress();
            instance.transform = transformComp.worldTransform;
            instance.instanceCustomIndex = instanceIndex++;
            instance.vertexBufferAddress = static_cast<const VulkanVertexBuffer&>(meshComp.mesh->getVertexBuffer()).getVulkanBuffer().getDeviceAddress();
            instance.indexBufferAddress = static_cast<const VulkanIndexBuffer&>(meshComp.mesh->getIndexBuffer()).getVulkanBuffer().getDeviceAddress();
            instance.materialIndex = meshComp.material ? meshComp.material->getIndex() : 0;
            instances.push_back(instance);
            if (transformComp.isDirty) {
                needsUpdate = true;
            }
            transformComp.isDirty = false;
        }
        m_renderer->getBackend()->updateTLAS(instances);
        if (instances.size() != m_prevInstanceSize) {
            needsUpdate = true;
        }
        m_prevInstanceSize = instances.size();

        if (!instances.empty()) {
            RTCameraUBO rtUBO{};
            rtUBO.invView = glm::inverse(ubo.view);
            rtUBO.invProj = glm::inverse(ubo.proj);
            rtUBO.frameIndex = m_frameCounter++;
            m_renderer->getBackend()->updateUniformBufferRT(rtUBO, needsUpdate); //TODO update
            m_renderer->getBackend()->updateRTDescriptorSet();
            m_renderer->getBackend()->dispatchRayTracing();
        }
    }

    m_renderer->getBackend()->beginRenderPass();

    m_renderer3D->beginScene();
    if (m_renderer->getBackend()->hasRTSupport() && m_useFullRT) {
        m_renderer->getBackend()->bindPipeline(VulkanUtils::PipelineType::PipelineRTDisplay);
        m_renderer->getBackend()->drawFullscreenTriangle();
    }

    if (!m_renderer->getBackend()->hasRTSupport() || !m_useFullRT) {
        glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(camera->getViewMatrix()));
        glm::mat4 skyProj = camera->getProjectionMatrix();
        skyProj[1][1] *= -1;
        SkyUniformBufferObject skyUbo{};
        skyUbo.invViewProj = glm::inverse(skyProj * viewNoTranslation);
        m_renderer->getBackend()->updateUniformBufferSky(skyUbo);
        m_renderer->getBackend()->drawFullscreenTriangle();
        m_renderer->getBackend()->bindPipeline(VulkanUtils::PipelineType::Pipeline3D);

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
                m_renderer3D->drawMesh(meshComponent.mesh, transform.worldTransform, meshComponent.albedoTexture, m_useHybridRT);
            }
        }
    }

    m_renderer3D->endScene();
}

std::shared_ptr<Material> RenderingServer::createMaterial(const MaterialParameters& params, const std::string& path) {
    return m_renderer3D->createMaterial(params, path);
}

std::shared_ptr<Mesh> RenderingServer::createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    if (vertices.empty() || indices.empty()) {
        QI_CORE_ERROR("createMesh called with empty vertex/index data — skipping mesh creation.");
        return nullptr;
    }
    std::shared_ptr<VertexBuffer> vertexBuffer = m_renderer->getBackend()->createVertexBuffer(vertices);
    std::shared_ptr<IndexBuffer> indexBuffer = m_renderer->getBackend()->createIndexBuffer(indices);

    std::unique_ptr<VulkanAccelerationStructure> blas = nullptr;

    if (m_renderer->getBackend()->hasRTSupport()) {
        blas = m_renderer->getBackend()->createBLAS(
            *vertexBuffer, static_cast<uint32_t>(vertices.size()), sizeof(Vertex),
            *indexBuffer, static_cast<uint32_t>(indices.size())
        );
    }

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vertexBuffer, indexBuffer);

    if (blas) {
        mesh->setBLAS(std::move(blas));
    }

    return mesh;
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

void RenderingServer::onEvent(Event& event) {
    if (event.getEventType() == EventType::UseFullRt) {
        UseFullRtEvent& e = static_cast<UseFullRtEvent&>(event);
        m_useFullRT = e.isEnabled();
    }

    if (event.getEventType() == EventType::UseRt) {
        UseRtEvent& e = static_cast<UseRtEvent&>(event);
        m_useHybridRT = e.isEnabled();
    }


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
