#include "Application.hpp"
#include "Base.hpp"
#include "Window.hpp"
#include "core/io/ResourceLoader.hpp"
#include "scene/Scene.hpp"
#include "servers/rendering/RenderingServer.hpp"
#include "utils/Time.hpp"
#include <GLFW/glfw3.h>



namespace Qi {

Application* Application::s_instance = nullptr;

Application::Application(const ApplicationSpecification& specification) : m_specification(specification), m_sceneManager(specification.windowWidth, specification.windowHeight) {
    QI_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    if (!m_specification.workingDirectory.empty()) {
        if (std::filesystem::exists(m_specification.workingDirectory)) {
            std::filesystem::current_path(m_specification.workingDirectory);
        }
        else {
            QI_CORE_ERROR("Working directory does not exist: {0}", m_specification.workingDirectory);
        }
    }

    m_window = Window::create(WindowProps(m_specification.name, m_specification.windowWidth, m_specification.windowHeight, m_specification.graphicsAPI));
    m_window->setEventCallback(QI_BIND_EVENT_FN(Application::onEvent));

    m_renderingServer = createScope<RenderingServer>(*m_window, m_specification.graphicsAPI);
    m_resourceLoader = createScope<ResourceLoader>(*m_renderingServer);
}

Application::~Application() {

}

void Application::run() {
    while (m_running) {

        float time = Time::getTime();
        Timestep timestep = time - m_lastFrameTime;
        m_lastFrameTime = time;

        if (!m_minimized) {
            float renderStart = Time::getTime();
            if (!m_renderingServer->beginFrame())
                continue;
            if (m_minimized)
                continue;

            float updateStart = Time::getTime();

            m_sceneManager.onUpdate(timestep);
            m_renderingServer->render(m_sceneManager.getCurrentScene());

            float updateEnd = Time::getTime();
            m_updateTimeMs = (updateEnd - updateStart) * 1000.0;

            m_renderingServer->beginImGui();
            m_renderingServer->getImGuiLayer().render(timestep);
            m_renderingServer->endImGui();

            m_renderingServer->endFrame();

            float renderEnd = Time::getTime();
            m_renderTimeMs = (renderEnd - renderStart) * 1000.0;
        }
        m_inputServer.update();
        m_window->onUpdate();

    }
}

void Application::setScene(std::unique_ptr<Scene> scene) {
    m_sceneManager.setScene(std::move(scene));
}

void Application::onEvent(Event& event) {
	EventDispatcher dispatcher(event);
	dispatcher.dispatch<WindowCloseEvent>(QI_BIND_EVENT_FN(Application::onWindowClose));
	dispatcher.dispatch<WindowResizeEvent>(QI_BIND_EVENT_FN(Application::onWindowResize));
	dispatcher.dispatch<VSyncEvent>(QI_BIND_EVENT_FN(Application::onVSync));

	m_renderingServer->onEvent(event);
    m_sceneManager.onEvent(event);
    m_inputServer.onEvent(event);
}

bool Application::onWindowClose(WindowCloseEvent& e) {
	m_running = false;
	return true;
}

bool Application::onVSync(VSyncEvent& e) {
    m_renderingServer->setVSync(e.isEnabled());
    return true;
}

bool Application::onWindowResize(WindowResizeEvent& e) {

    if (e.getWidth() == 0 || e.getHeight() == 0) {
        m_minimized = true;
        return false;
    }

    m_minimized = false;

    m_renderingServer->onWindowResize(e.getWidth(), e.getHeight());

    return false;
}

}
