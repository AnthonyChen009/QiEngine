#include "LinuxWindow.hpp"
#include "Window.hpp"
#include "internal/qipch.hpp"
#include "events/ApplicationEvent.hpp"
#include "Log.hpp"
#include "Assert.hpp"

namespace Qi {
static uint8_t s_GLFWWindowCount = 0;

static void GLFWErrorCallback(int error, const char* description)
{
	QI_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

LinuxWindow::LinuxWindow(const WindowProps& props) {
    init(props);
}

LinuxWindow::~LinuxWindow() {
    shutdown();
}

void LinuxWindow::init(const WindowProps& props) {
    m_Data.title = props.title;
	m_Data.width = props.width;
	m_Data.height = props.height;
	QI_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

	if (s_GLFWWindowCount == 0) {
		int success = glfwInit();
		QI_CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
	}

	m_Window = glfwCreateWindow((int)props.width, (int)props.height, m_Data.title.c_str(), nullptr, nullptr);
	QI_CORE_ASSERT(m_Window, "Failed to create GLFW window!");
	s_GLFWWindowCount++;

	glfwSetWindowUserPointer(m_Window, &m_Data);
	setVSync(true);

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.width = width;
		data.height = height;

		WindowResizeEvent event(width, height);
		if (data.eventCallback)
            data.eventCallback(event);

	});

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		if (data.eventCallback)
            data.eventCallback(event);
	});
}
void LinuxWindow::shutdown()
{
    if (!m_Window)
        return;

    glfwDestroyWindow(m_Window);
    m_Window = nullptr;

    --s_GLFWWindowCount;

    if (s_GLFWWindowCount == 0)
        glfwTerminate();
}

void LinuxWindow::onUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}

void LinuxWindow::setVSync(bool enabled)
{
	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	m_Data.VSync = enabled;
}

bool LinuxWindow::isVSync() const {
    return m_Data.VSync;
}


}
