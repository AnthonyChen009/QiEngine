#include "RenderingServer.hpp"
#include "core/Base.hpp"
#include "managers/imgui/ImGuiManager.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Renderer2D.hpp"
#include "scene/Components.hpp"
#include <memory>

namespace Qi {

RenderingServer::RenderingServer(Window& window, GraphicsAPI graphicsAPI) {
    m_renderer = createScope<Renderer>(graphicsAPI);
    m_renderer->init(window, graphicsAPI);

    m_renderer2D = createScope<Renderer2D>(*m_renderer);

    m_imGuiManager = createScope<ImGuiManager>(window, *m_renderer);
    m_imGuiManager->init();
}

RenderingServer::~RenderingServer() {
    m_imGuiManager->shutdown();
    m_renderer->shutdown();
}

bool RenderingServer::beginFrame() {
    return m_renderer->beginFrame();
}

void RenderingServer::render(Scene& scene) {
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
    m_imGuiManager->onEvent(event);
}

std::shared_ptr<Texture2D> RenderingServer::createTexture2D(const std::string& path) {
    return m_renderer2D->loadTexture(path);
}

void RenderingServer::beginImGui() {
    m_imGuiManager->begin();
}

void RenderingServer::endImGui() {
    m_imGuiManager->end();
}

ImGuiManager& RenderingServer::getImGuiManager() {
    return *m_imGuiManager;
}


}
