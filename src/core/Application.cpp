#include "Application.hpp"
#include "Base.hpp"
#include "renderer/Renderer.hpp"
#include "Window.hpp"
#include "renderer/vulkan/VulkanRenderer.hpp"
#include "utils/Time.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Renderer.hpp"


namespace Qi {

Application* Application::s_instance = nullptr;

Application::Application(const ApplicationSpecification& specification) : m_specification(specification) {

    QI_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    if (!m_specification.workingDirectory.empty())
        std::filesystem::current_path(m_specification.workingDirectory);

    m_window = Window::create(WindowProps(m_specification.name, m_specification.windowWidth, m_specification.windowHeight, m_specification.graphicsAPI));
    m_window->setEventCallback(QI_BIND_EVENT_FN(Application::onEvent));


    Renderer::init(*m_window, m_specification.graphicsAPI);

    m_imGuiLayer = new ImGuiLayer();

    pushLayer(m_imGuiLayer);
}

Application::~Application() {
    m_layerStack.popLayer(m_imGuiLayer); // remove from stack first
    m_imGuiLayer->onDetach();
    Renderer::shutdown();
}

void Application::run() {
    while (m_running) {

        float time = Time::getTime();
        Timestep timestep = time - m_lastFrameTime;
        m_lastFrameTime = time;

        if (!m_minimized) {
            float renderStart = Time::getTime();
            if (!Renderer::beginFrame())
                continue;
            if (m_minimized)
                continue;

            float updateStart = Time::getTime();

            for (Layer* layer : m_layerStack) {
                layer->onUpdate(timestep);
            }

            float updateEnd = Time::getTime();
            m_updateTimeMs = (updateEnd - updateStart) * 1000.0;

            m_imGuiLayer->begin();
            for (Layer* layer : m_layerStack)
                layer->onImGuiRender();
            m_imGuiLayer->end();

            Renderer::endFrame();

            float renderEnd = Time::getTime();
            m_renderTimeMs = (renderEnd - renderStart) * 1000.0;
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
	dispatcher.dispatch<VSyncEvent>(QI_BIND_EVENT_FN(Application::onVSync));

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

bool Application::onVSync(VSyncEvent& e) {
    Renderer::setVSync(e.isEnabled());
    return true;
}

bool Application::onWindowResize(WindowResizeEvent& e) {

    if (e.getWidth() == 0 || e.getHeight() == 0) {
        m_minimized = true;
        return false;
    }

    m_minimized = false;

    Renderer::onWindowResize(e.getWidth(), e.getHeight());

    return false;
}

}
