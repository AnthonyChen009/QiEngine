#include "Renderer.hpp"
#include "vulkan/VulkanRenderer.hpp"
#include "core/Base.hpp"
#include "core/Assert.hpp"

namespace Qi {

Renderer::Renderer(GraphicsAPI api) {
    switch (api) {
        case GraphicsAPI::Vulkan:
            m_backend = createScope<VulkanRenderer>();
            break;

        case GraphicsAPI::OpenGL:
            QI_CORE_ASSERT("OpenGL not supported");
            break;
        case GraphicsAPI::None:

            break;
    }
}

void Renderer::init(Window &window, GraphicsAPI api) {
    QI_CORE_ASSERT(m_backend, "Renderer backend not initialized!");
    m_backend->init(window);
}

bool Renderer::beginFrame() {
    QI_CORE_ASSERT(m_backend, "Renderer backend not initialized!");
    return m_backend->beginFrame();
}

void Renderer::endFrame() {
    QI_CORE_ASSERT(m_backend, "Renderer backend not initialized!");
    m_backend->endFrame();
}

void Renderer::updateUniformBuffer() {
    Renderer::getBackend()->updateUniformBuffer();
}

void Renderer::bindPipeline() {
    //Renderer::getBackend()->bindPipeline();
}

void Renderer::setVSync(bool isVSync) {
    m_backend->onVysncToggle(isVSync);
}

void Renderer::drawQuad() {
    //s_instance->m_backend->updateUniformBuffer();
    //s_instance->m_backend->bindPipeline();
    m_backend->drawIndexed(6);
}

void Renderer::drawIndexed() {
    QI_CORE_ASSERT(m_backend, "Renderer backend not initialized!");
    m_backend->drawIndexed(6);
}

void Renderer::onWindowResize(uint32_t width, uint32_t height) {
    m_backend->onWindowResize(width, height);
}

RendererBackend* Renderer::getBackend() {
    return m_backend.get();
}

void Renderer::shutdown() {
    m_backend->shutdown();
    m_backend.reset();
}


}
