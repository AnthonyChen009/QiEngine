#pragma once

#define GLFW_INCLUDE_VULKAN
#include "core/Window.hpp"
#include <GLFW/glfw3.h>
#include <string>

namespace Qi {

class LinuxWindow : public Window {
public:
    LinuxWindow(const WindowProps& props);
    ~LinuxWindow();

    void onUpdate() override;

    unsigned int getWidth() const override { return m_data.width; }
	unsigned int getHeight() const override { return m_data.height; }
	std::string getWindowName() const override {return m_data.title;}

	void setEventCallback(const EventCallbackFn& callback) override { m_data.eventCallback = callback; }
    void setVSync(bool enabled) override;
    bool isVSync() const override;
    void waitForValidFramebufferSize() override;
    void* getNativeWindow() const override { return m_window; }

private:
    void init(const WindowProps& props);
    void shutdown();

private:
    GLFWwindow* m_window = nullptr;

    struct WindowData {
		std::string title = "Qi Engine";
		unsigned int width = 1280;
		unsigned int height = 720;
		GraphicsAPI graphicsAPI = GraphicsAPI::Vulkan;
		bool VSync = false;

		EventCallbackFn eventCallback;
	};

    WindowData m_data;

};

}
