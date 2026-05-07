#include "Renderer.hpp"
#include "vulkan/VulkanRenderer.hpp"
#include "opengl/OpenGLRenderer.hpp"
#include "Base.hpp"
#include "Assert.hpp"

namespace Qi {

Renderer::Renderer(GraphicsAPI api) {
    switch (api) {
        case GraphicsAPI::Vulkan:
            m_backend = createScope<VulkanRenderer>();
            break;

        case GraphicsAPI::OpenGL:
            m_backend = createScope<OpenGLRenderer>();
            break;
        case GraphicsAPI::None:

            break;
    }
}

Scope<Renderer> Renderer::s_instance = nullptr;

void Renderer::init(Window &window, GraphicsAPI api) {
    s_instance = createScope<Renderer>(api);
    QI_CORE_ASSERT(s_instance->m_backend, "Renderer backend not initialized!");
    s_instance->m_backend->init(window);
}

bool Renderer::beginFrame() {
    QI_CORE_ASSERT(s_instance->m_backend, "Renderer backend not initialized!");
    return s_instance->m_backend->beginFrame();
}

void Renderer::endFrame() {
    QI_CORE_ASSERT(s_instance->m_backend, "Renderer backend not initialized!");
    s_instance->m_backend->endFrame();
}

void Renderer::drawQuad() {
    s_instance->m_backend->updateUniformBuffer();
    s_instance->m_backend->bindPipeline();
    s_instance->m_backend->drawIndexed(6);
}

void Renderer::drawIndexed() {
    QI_CORE_ASSERT(s_instance->m_backend, "Renderer backend not initialized!");
    s_instance->m_backend->drawIndexed(6);
}

void Renderer::onWindowResize(uint32_t width, uint32_t height) {
    s_instance->m_backend->onWindowResize(width, height);
}

RendererBackend* Renderer::getBackend() {
    return s_instance->m_backend.get();
}

void Renderer::shutdown() {
    s_instance->m_backend->shutdown();
}


}
