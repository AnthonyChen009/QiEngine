#include "Renderer.hpp"
#include "vulkan/VulkanRenderer.hpp"
#include "Base.hpp"
#include "Assert.hpp"

namespace Qi {

Renderer::Renderer(GraphicsAPI api) {
    switch (api) {
        case GraphicsAPI::Vulkan:
            m_backend = createScope<VulkanRenderer>();
            break;

        case GraphicsAPI::OpenGL:
            //m_backend = CreateScope<OpenGLRenderer>();
            break;
        case GraphicsAPI::None:

            break;
    }
}

void Renderer::init(Window &window) {
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

void Renderer::drawQuad() {
    m_backend->updateUniformBuffer();
    m_backend->bindPipeline();
    m_backend->drawIndexed();
}

void Renderer::drawIndexed() {
    QI_CORE_ASSERT(m_backend, "Renderer backend not initialized!");
    m_backend->drawIndexed();
}


void Renderer::onWindowResize(uint32_t width, uint32_t height) {
    m_backend->onWindowResize(width, height);
}


void Renderer::shutdown() {
    m_backend->shutdown();
}


}
