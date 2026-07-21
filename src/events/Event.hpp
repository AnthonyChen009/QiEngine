#pragma once

#include "core/Base.hpp"
#include <string>

namespace Qi {

enum class EventType {
    None = 0,
    WindowClose,
    WindowResize,
    WindowMinimized,
    WindowRestored,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    AppTick,
    AppUpdate,
    AppRender,
    KeyPressed,
    KeyReleased,
    KeyTyped,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,
    VSync,
};

enum EventCategory {
    None = 0,
    eventCategoryApplication = BIT(0),
    eventCategoryInput       = BIT(1),
    eventCategoryKeyboard    = BIT(2),
    eventCategoryMouse       = BIT(3),
    eventCategoryMouseButton = BIT(4)
};

#define QI_EVENT_CLASS_TYPE(type)                       \
    static EventType getStaticType() {                  \
        return EventType::type;                         \
    }                                                   \
    EventType getEventType() const override {           \
        return getStaticType();                         \
    }                                                   \
    const char* getName() const override {              \
        return #type;                                   \
    }

#define QI_EVENT_CLASS_CATEGORY(category)               \
    int getCategoryFlags() const override {             \
        return category;                                \
    }

class Event {
public:
    virtual ~Event() = default;

    bool handled = false;

    virtual EventType getEventType() const = 0;
    virtual const char* getName() const = 0;
    virtual int getCategoryFlags() const = 0;

    virtual std::string toString() const {
        return getName();
    }

    bool isInCategory(EventCategory category) const {
        return getCategoryFlags() & category;
    }
};

class EventDispatcher {
public:
    EventDispatcher(Event& event)
        : m_event(event) {}

    template<typename T, typename F>
    bool dispatch(const F& func) {
        if (m_event.getEventType() == T::getStaticType()) {
            m_event.handled |= func(static_cast<T&>(m_event));
            return true;
        }

        return false;
    }
private:
    Event& m_event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& event) {
    return os << event.toString();
}

}
