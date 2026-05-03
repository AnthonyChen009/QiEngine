#include "Application.hpp"
#include "Base.hpp"



namespace Qi {

Application::Application() {
    m_window = Window::create();
    m_window->setEventCallback(QI_BIND_EVENT_FN(Application::onEvent));
}

Application::~Application() {

}

void Application::run() {
    while (m_running) {
        m_window->onUpdate();
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

	return false;
}

}
