#pragma once

#include "Event.hpp"
#include <sstream>

namespace Qi {

class WindowResizeEvent : public Event {
public:
    WindowResizeEvent(unsigned int width, unsigned int height)
        : m_width(width), m_height(height) {}

    unsigned int getWidth() const { return m_width; }
    unsigned int getHeight() const { return m_height; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << m_width << ", " << m_height;
        return ss.str();
    }

    QI_EVENT_CLASS_TYPE(WindowResize)
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)

private:
    unsigned int m_width, m_height;
};

class WindowCloseEvent : public Event {
public:
    WindowCloseEvent() = default;

    QI_EVENT_CLASS_TYPE(WindowClose)
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

class WindowMinimizedEvent : public Event {
public:
    WindowMinimizedEvent() = default;
    std::string toString() const override { return "WindowMinimizedEvent"; }
    QI_EVENT_CLASS_TYPE(WindowMinimized)
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

class WindowRestoredEvent : public Event {
public:
    WindowRestoredEvent() = default;
    std::string toString() const override { return "WindowRestoredEvent"; }
    QI_EVENT_CLASS_TYPE(WindowRestored)
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

class AppTickEvent : public Event {
public:
    AppTickEvent() = default;

    QI_EVENT_CLASS_TYPE(AppTick)
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

class AppUpdateEvent : public Event {
public:
    AppUpdateEvent() = default;

    QI_EVENT_CLASS_TYPE(AppUpdate)
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

class AppRenderEvent : public Event {
public:
    AppRenderEvent() = default;

    QI_EVENT_CLASS_TYPE(AppRender)
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

}
