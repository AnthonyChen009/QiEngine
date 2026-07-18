#pragma once

#include "Base.hpp"
#include "events/Event.hpp"
#include <sstream>
#include <cstdint>
#include <functional>

namespace Qi {
    using RawEventCallback = std::function<void(void*)>;

    enum class GraphicsAPI {
        None = 0,
        OpenGL,
        Vulkan
    };
    struct WindowProps {
        std::string title = "Qi Engine";
        uint32_t width = 1280;
        uint32_t height = 720;
        GraphicsAPI graphicsAPI = GraphicsAPI::Vulkan;
    };
    class Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void onUpdate() = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual std::string getWindowName() const = 0;

		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;
		virtual void waitForValidFramebufferSize() = 0;

		static Scope<Window> create(const WindowProps& props = WindowProps());

		virtual void setRawEventCallback(const RawEventCallback& callback) = 0;
    };
}
