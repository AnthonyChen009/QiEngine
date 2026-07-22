#pragma once

#include "Base.hpp"
#include "core/io/ResourceLoader.hpp"
#include "events/Event.hpp"
#include "internal/qipch.hpp"
#include "Window.hpp"
#include "events/ApplicationEvent.hpp"
#include "core/Assert.hpp"
#include "events/RenderingEvents.hpp"
#include "scene/SceneManager.hpp"
#include "scene/Scene.hpp"
#include "servers/input/InputServer.hpp"
#include "servers/rendering/RenderingServer.hpp"

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

    void onEvent(Event& event);

	void setScene(std::unique_ptr<Scene> scene);

	Window& getWindow() { return *m_window; }
	RenderingServer& getRenderingServer() { return *m_renderingServer; }
	ResourceLoader& getResourceLoader() { return *m_resourceLoader; }
	InputServer& getInputServer () { return m_inputServer; }

    const ApplicationSpecification& GetSpecification() const { return m_specification; }

public:
    static Application& get() { return *s_instance; }
    double getUpdateTimeMs() const { return m_updateTimeMs; }
    double getRenderTimeMs() const { return m_renderTimeMs; }

private:
    void run();
    bool onWindowClose(WindowCloseEvent& e);
	bool onWindowResize(WindowResizeEvent& e);
	bool onVSync(VSyncEvent& e);
    bool onWindowMinimized(WindowMinimizedEvent& e);
    bool onWindowRestored(WindowRestoredEvent& e);
private:
    Scope<Window> m_window;
    InputServer m_inputServer;
    ApplicationSpecification m_specification;
    Scope<RenderingServer> m_renderingServer;
    Scope<ResourceLoader> m_resourceLoader;
    SceneManager m_sceneManager;
    bool m_running = true;
    bool m_minimized = false;
    float m_lastFrameTime = 0.0f;
    float m_updateTimeMs = 0.0f;
    float m_renderTimeMs = 0.0f;
    float m_fixedTimestep = 1.0f / 60.0f;
    float m_fixedUpdateAccumulator = 0.0f;
    const int m_maxPhysicsSteps = 5;
    int m_physicsSteps = 0;

private:
    static Application* s_instance;
    friend int::main(int argc, char** argv);
};

std::unique_ptr<Application> createApplication(Qi::ApplicationCommandLineArgs args);

}
