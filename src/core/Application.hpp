#pragma once

#include "Base.hpp"
#include "LayerStack.hpp"
#include "renderer/RendererBackend.hpp"
#include "events/Event.hpp"
#include "internal/qipch.hpp"
#include "Window.hpp"
#include "events/ApplicationEvent.hpp"
#include "core/Assert.hpp"
#include "imgui/ImGuiLayer.hpp"

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

    void pushLayer(Layer* layer);
	void pushOverlay(Layer* layer);

	Window& getWindow() { return *m_window; }

    const ApplicationSpecification& GetSpecification() const { return m_specification; }

public:
    static Application& get() { return *s_instance; }
    ImGuiLayer* getImGuiLayer() { return m_imGuiLayer; }
    double getUpdateTimeMs() const { return m_updateTimeMs; }
    double getRenderTimeMs() const { return m_renderTimeMs; }

private:
    void run();
    bool onWindowClose(WindowCloseEvent& e);
	bool onWindowResize(WindowResizeEvent& e);
private:
    LayerStack m_layerStack;
    ApplicationSpecification m_specification;
    Scope<Window> m_window;
    bool m_running = true;
    bool m_minimized = false;
    float m_lastFrameTime = 0.0f;
    ImGuiLayer* m_imGuiLayer;
    float m_updateTimeMs = 0.0f;
    float m_renderTimeMs = 0.0f;


private:
    static Application* s_instance;
    friend int::main(int argc, char** argv);
};

std::unique_ptr<Application> createApplication(Qi::ApplicationCommandLineArgs args);

}
