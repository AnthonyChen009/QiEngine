#pragma once
#include "Event.hpp"
#include <sstream>

namespace Qi {

class VSyncEvent : public Event {
public:
    VSyncEvent(bool enabled) : m_enabled(enabled) {}
    bool isEnabled() const { return m_enabled; }
    std::string toString() const override {
        std::stringstream ss;
		ss << "VSync Event: " << m_enabled;
		return ss.str();
    }
    QI_EVENT_CLASS_TYPE(VSync);
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)
private:
    bool m_enabled;
};

class UseFullRtEvent : public Event {
public:
    UseFullRtEvent(bool enabled) : m_enabled(enabled) {}
    bool isEnabled() const { return m_enabled; }
    std::string toString() const override {
        std::stringstream ss;
		ss << "UseFullRt Event: " << m_enabled;
		return ss.str();
    }
    QI_EVENT_CLASS_TYPE(UseFullRt);
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)
private:
    bool m_enabled;
};

class UseRtEvent : public Event {
public:
    UseRtEvent(bool enabled) : m_enabled(enabled) {}
    bool isEnabled() const { return m_enabled; }
    std::string toString() const override {
        std::stringstream ss;
		ss << "UseFullRt Event: " << m_enabled;
		return ss.str();
    }
    QI_EVENT_CLASS_TYPE(UseRt);
    QI_EVENT_CLASS_CATEGORY(eventCategoryApplication)
private:
    bool m_enabled;
};


}
