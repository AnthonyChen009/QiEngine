#include "WindowsWindow.hpp"
#include "core/Window.hpp"
#include "internal/qipch.hpp"
#include "events/ApplicationEvent.hpp"
#include "core/Log.hpp"
#include "core/Assert.hpp"
#include "events/KeyEvent.hpp"
#include <SDL3/SDL_vulkan.h>
#include "events/MouseEvent.hpp"

namespace Qi {
static uint8_t s_windowCount = 0;

static void glfwErrorCallback(int error, const char* description) {
	QI_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

WindowsWindow::WindowsWindow(const WindowProps& props) {
    init(props);
}

WindowsWindow::~WindowsWindow() {
    shutdown();
}

void WindowsWindow::init(const WindowProps& props) {
    m_data.title = props.title;
	m_data.width = props.width;
	m_data.height = props.height;
	m_data.graphicsAPI = props.graphicsAPI;

	QI_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);


	if (s_windowCount == 0) {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
            QI_CORE_ERROR("SDL_Init failed: {}", SDL_GetError());
            QI_CORE_ASSERT(false, "SDL_Init failed!");
        }
	}

	if (!SDL_Vulkan_LoadLibrary(nullptr)) {
        QI_CORE_ERROR("Could not load Vulkan: {}", SDL_GetError());
        QI_CORE_ASSERT(false, "Vulkan is not supported on this system.");
	}

    m_data.graphicsAPI = GraphicsAPI::Vulkan;

	m_window = SDL_CreateWindow(m_data.title.c_str(), props.width, props.height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	QI_CORE_ASSERT(m_window, "Failed to create SDL window!");
	s_windowCount++;

	SDL_SetPointerProperty(SDL_GetWindowProperties(m_window), "user_data", &m_data);
	setVSync(true);

	SDL_ShowWindow(m_window);
}

void WindowsWindow::shutdown() {
    if (!m_window)
        return;

    m_rawEventCallback = nullptr;

    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    --s_windowCount;

    if (s_windowCount == 0)
        SDL_Quit();
}

void WindowsWindow::setRawEventCallback(const RawEventCallback& callback) {
    m_rawEventCallback = callback;
}

void WindowsWindow::setFullscreen(bool fullscreen) {
    SDL_SetWindowFullscreen(m_window, fullscreen);
}

void WindowsWindow::onUpdate() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (m_rawEventCallback)
            m_rawEventCallback(&event);

        switch (event.type) {
            case SDL_EVENT_WINDOW_RESIZED: {
                m_data.width = event.window.data1;
                m_data.height = event.window.data2;
                WindowResizeEvent resizeEvent(event.window.data1, event.window.data2);
                if (m_data.eventCallback) m_data.eventCallback(resizeEvent);
                break;
            }
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                WindowCloseEvent closeEvent;
                if (m_data.eventCallback) m_data.eventCallback(closeEvent);
                break;
            }
            case SDL_EVENT_KEY_DOWN: {
                if (event.key.repeat) {
                    KeyPressedEvent keyEvent(event.key.key, true);
                    if (m_data.eventCallback) m_data.eventCallback(keyEvent);
                } else {
                    KeyPressedEvent keyEvent(event.key.key, false);
                    if (m_data.eventCallback) m_data.eventCallback(keyEvent);
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                KeyReleasedEvent keyEvent(event.key.key);
                if (m_data.eventCallback) m_data.eventCallback(keyEvent);
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                MouseButtonPressedEvent btnEvent(event.button.button);
                if (m_data.eventCallback) m_data.eventCallback(btnEvent);
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                MouseButtonReleasedEvent btnEvent(event.button.button);
                if (m_data.eventCallback) m_data.eventCallback(btnEvent);
                break;
            }
            case SDL_EVENT_MOUSE_MOTION: {
                MouseMovedEvent moveEvent(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                if (m_data.eventCallback) m_data.eventCallback(moveEvent);
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: {
                MouseScrolledEvent scrollEvent(event.wheel.x, event.wheel.y);
                if (m_data.eventCallback) m_data.eventCallback(scrollEvent);
                break;
            }
            case SDL_EVENT_WINDOW_MINIMIZED: {
                WindowMinimizedEvent event;
                if (m_data.eventCallback) m_data.eventCallback(event);
                break;
            }
            case SDL_EVENT_WINDOW_RESTORED: {
                WindowRestoredEvent event;
                if (m_data.eventCallback) m_data.eventCallback(event);
                break;
            }
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: {
                WindowFullscreenEvent event;
                if (m_data.eventCallback) m_data.eventCallback(event);
                break;
            }
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: {
                WindowLeaveFullscreenEvent event;
                if (m_data.eventCallback) m_data.eventCallback(event);
                break;
            }
        }
    }
}

void WindowsWindow::setVSync(bool enabled)
{
	m_data.VSync = enabled;

	if (m_data.graphicsAPI == GraphicsAPI::OpenGL) {

    }
}

void WindowsWindow::waitForValidFramebufferSize() {
    int width = 0, height = 0;
    SDL_GetWindowSizeInPixels(m_window, &width, &height);

    while (width == 0 || height == 0) {
        SDL_WaitEvent(nullptr);
        SDL_GetWindowSizeInPixels(m_window, &width, &height);
    }

    m_data.width = width;
    m_data.height = height;
}

bool WindowsWindow::isVSync() const {
    return m_data.VSync;
}

}
