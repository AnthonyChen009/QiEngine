#pragma once

#include "LayerStack.hpp"
#include "events/Event.hpp"
#include "internal/qipch.hpp"
#include "Window.hpp"
#include "events/ApplicationEvent.hpp"


namespace Qi {

class Application {
public:
    Application();
    virtual ~Application();

    void onEvent(Event& e);

    void run();
private:
    bool onWindowClose(WindowCloseEvent& e);
	bool onWindowResize(WindowResizeEvent& e);
private:
    Scope<Window> m_window;
    bool m_running = true;
    bool m_minimized = false;
    LayerStack m_layerStack;
    float m_lastFrameTime = 0.0f;
};

std::unique_ptr<Application> createApplication();

}
