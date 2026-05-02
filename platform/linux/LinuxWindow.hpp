#pragma once

#include "Window.hpp"
#include <GLFW/glfw3.h>
#include <string>

namespace Qi {

class LinuxWindow : public Window {
public:
    LinuxWindow(const WindowProps& props);
    ~LinuxWindow();

    void onUpdate() override;

    unsigned int getWidth() const override { return m_Data.width; }
	unsigned int getHeight() const override { return m_Data.height; }

	void setEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }
    void setVSync(bool enabled) override;
    bool isVSync() const override;

    void* getNativeWindow() const override { return m_Window; }

private:
    void init(const WindowProps& props);
    void shutdown();

private:
    GLFWwindow* m_Window = nullptr;

    struct WindowData {
		std::string title = "Qi Engine";
		unsigned int width = 1280;
		unsigned int height = 720;
		bool VSync = false;

		EventCallbackFn eventCallback;
	};

    WindowData m_Data;

};

}
