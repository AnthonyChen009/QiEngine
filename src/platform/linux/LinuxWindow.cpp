#include "LinuxWindow.hpp"
#include "core/Window.hpp"
#include "internal/qipch.hpp"
#include "events/ApplicationEvent.hpp"
#include "core/Log.hpp"
#include "core/Assert.hpp"
#include <GLFW/glfw3.h>
#include "events/KeyEvent.hpp"
#include "events/MouseEvent.hpp"

namespace Qi {
static uint8_t s_glfwWindowCount = 0;

static void glfwErrorCallback(int error, const char* description) {
	QI_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

LinuxWindow::LinuxWindow(const WindowProps& props) {
    init(props);
}

LinuxWindow::~LinuxWindow() {
    shutdown();
}

void LinuxWindow::init(const WindowProps& props) {
    m_data.title = props.title;
	m_data.width = props.width;
	m_data.height = props.height;
	m_data.graphicsAPI = props.graphicsAPI;

	QI_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);
	//glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

	if (s_glfwWindowCount == 0) {
		int success = glfwInit();
		QI_CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(glfwErrorCallback);
	}

	if (!glfwVulkanSupported()) {
        QI_CORE_ASSERT("Vulkan is not supported on this system.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_data.graphicsAPI = GraphicsAPI::Vulkan;

	m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
	QI_CORE_ASSERT(m_window, "Failed to create GLFW window!");
	s_glfwWindowCount++;

    if (m_data.graphicsAPI == GraphicsAPI::OpenGL) {
        glfwMakeContextCurrent(m_window);
    }

	glfwSetWindowUserPointer(m_window, &m_data);
	setVSync(true);

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.width = width;
        data.height = height;

        WindowResizeEvent event(width, height);
        if (data.eventCallback)
            data.eventCallback(event);
    });

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		if (data.eventCallback)
            data.eventCallback(event);
	});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, false);
                if (data.eventCallback) data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                if (data.eventCallback) data.eventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, true);
                if (data.eventCallback) data.eventCallback(event);
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                if (data.eventCallback) data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                if (data.eventCallback) data.eventCallback(event);
                break;
            }
        }
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseMovedEvent event((float)xPos, (float)yPos);
        if (data.eventCallback) data.eventCallback(event);
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseScrolledEvent event((float)xOffset, (float)yOffset);
        if (data.eventCallback) data.eventCallback(event);
    });

	glfwShowWindow(m_window);
}
void LinuxWindow::shutdown() {
    if (!m_window)
        return;

    glfwDestroyWindow(m_window);
    m_window = nullptr;

    --s_glfwWindowCount;

    if (s_glfwWindowCount == 0)
        glfwTerminate();
}

void LinuxWindow::onUpdate() {
    glfwPollEvents();
}

void LinuxWindow::setVSync(bool enabled)
{
	m_data.VSync = enabled;

	if (m_data.graphicsAPI == GraphicsAPI::OpenGL) {
        glfwSwapInterval(enabled ? 1 : 0);
    }
}

void LinuxWindow::waitForValidFramebufferSize() {
    int width = 0;
    int height = 0;

    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_window, &width, &height);
        glfwWaitEvents();
    }

    m_data.width = width;
    m_data.height = height;
}

bool LinuxWindow::isVSync() const {
    return m_data.VSync;
}

}
