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

}
