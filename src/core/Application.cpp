#include "Application.hpp"
#include "Base.hpp"
#include "renderer/Renderer.hpp"
#include "Window.hpp"
#include "renderer/vulkan/VulkanRenderer.hpp"
#include "utils/Time.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Renderer2D.hpp"

namespace Qi {

Application::Application(const ApplicationSpecification& specification) : m_specification(specification) {

    if (!m_specification.workingDirectory.empty())
        std::filesystem::current_path(m_specification.workingDirectory);

    m_window = Window::create(WindowProps(m_specification.name, m_specification.windowWidth, m_specification.windowHeight, m_specification.graphicsAPI));
    m_window->setEventCallback(QI_BIND_EVENT_FN(Application::onEvent));


    Renderer2D::init(*m_window, m_specification.graphicsAPI);
}

Application::~Application() {
    Renderer2D::shutdown();
}

void Application::run() {
    while (m_running) {

        float time = Time::GetTime();
        Timestep timestep = time - m_lastFrameTime;
        m_lastFrameTime = time;

        if (!m_minimized) {
            if (!Renderer2D::beginFrame())
                continue;
            if (m_minimized)
                continue;
            Renderer2D::updateUniformBuffer();
            Renderer2D::bindPipeline();
            for (Layer* layer : m_layerStack)
                layer->onUpdate(timestep);
            Renderer2D::endFrame();
        }
        m_window->onUpdate();
    }
}

void Application::pushLayer(Layer* layer) {
	m_layerStack.pushLayer(layer);
	layer->onAttach();
}

void Application::pushOverlay(Layer* layer) {
	m_layerStack.pushOverlay(layer);
	layer->onAttach();
}

void Application::onEvent(Event& event) {
	EventDispatcher dispatcher(event);
	dispatcher.dispatch<WindowCloseEvent>(QI_BIND_EVENT_FN(Application::onWindowClose));
	dispatcher.dispatch<WindowResizeEvent>(QI_BIND_EVENT_FN(Application::onWindowResize));

	for (auto it = m_layerStack.end(); it != m_layerStack.begin(); ){
		(*--it)->onEvent(event);
		if (event.handled)
			break;
	}
	if (event.handled)
		return;
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

    Renderer2D::onWindowResize(e.getWidth(), e.getHeight());

    return false;
}

}
