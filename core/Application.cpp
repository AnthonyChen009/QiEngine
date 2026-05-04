#include "Application.hpp"
#include "Base.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "vulkan/VulkanRenderer.hpp"


namespace Qi {

Application::Application(const ApplicationSpecification& specification) : m_specification(specification) {

    if (!m_specification.workingDirectory.empty())
        std::filesystem::current_path(m_specification.workingDirectory);

    m_window = Window::create(WindowProps(m_specification.name, m_specification.windowWidth, m_specification.windowHeight, m_specification.graphicsAPI));
    m_window->setEventCallback(QI_BIND_EVENT_FN(Application::onEvent));

    m_renderer = createScope<Renderer>(m_specification.graphicsAPI);
    m_renderer->init(*m_window);
}

Application::~Application() {

}

void Application::run() {
    while (m_running) {
        m_window->onUpdate();
        if (!m_renderer->beginFrame())
            continue; // or continue
        m_renderer->bindPipeline();
        m_renderer->draw(4);
        m_renderer->endFrame();
    }
}
void Application::onEvent(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowCloseEvent>(QI_BIND_EVENT_FN(Application::onWindowClose));
	dispatcher.dispatch<WindowResizeEvent>(QI_BIND_EVENT_FN(Application::onWindowResize));

	for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it) {
		if (e.handled)
			break;
		(*it)->onEvent(e);
	}
}

bool Application::onWindowClose(WindowCloseEvent& e) {
	m_running = false;
	return true;
}

bool Application::onWindowResize(WindowResizeEvent& e) {

    if (e.getWidth() == 0 || e.getHeight() == 0) {
        m_minimized = true;
        return false;
    }

    m_minimized = false;

    m_renderer->onWindowResize(e.getWidth(), e.getHeight());

    return false;
}

}
