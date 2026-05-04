#pragma once

#include "Base.hpp"
#include "LayerStack.hpp"
#include "Renderer.hpp"
#include "RendererBackend.hpp"
#include "events/Event.hpp"
#include "internal/qipch.hpp"
#include "Window.hpp"
#include "events/ApplicationEvent.hpp"
#include "Assert.hpp"

int main(int argc, char** argv);

namespace Qi {
struct ApplicationCommandLineArgs {
	int Count = 0;
	char** Args = nullptr;

	const char* operator[](int index) const {
		QI_CORE_ASSERT(index < Count);
		return Args[index];
	}
};

struct ApplicationSpecification {
	std::string name = "Qi Application";
	uint32_t windowWidth = 1280;
    uint32_t windowHeight = 720;
    GraphicsAPI graphicsAPI = GraphicsAPI::Vulkan;
	std::string workingDirectory;
	ApplicationCommandLineArgs commandLineArgs;
};

class Application {
public:
    Application(const ApplicationSpecification& specification);
    virtual ~Application();

    void onEvent(Event& e);
    const ApplicationSpecification& GetSpecification() const { return m_specification; }

private:
    void run();
    bool onWindowClose(WindowCloseEvent& e);
	bool onWindowResize(WindowResizeEvent& e);
private:
    ApplicationSpecification m_specification;
    Scope<Window> m_window;
    bool m_running = true;
    bool m_minimized = false;
    LayerStack m_layerStack;
    float m_lastFrameTime = 0.0f;
    Scope<Renderer> m_renderer;
private:
    friend int::main(int argc, char** argv);
};

std::unique_ptr<Application> createApplication(Qi::ApplicationCommandLineArgs args);

}
