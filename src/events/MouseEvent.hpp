#pragma once

#include "events/Event.hpp"
#include "core/MouseCodes.hpp"

namespace Qi {

class MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(const float x, const float y) : m_mouseX(x), m_mouseY(y) {}

	float getX() const {
	    return m_mouseX;
	}

	float getY() const {
	    return m_mouseY;
	}

	std::string toString() const override {
		std::stringstream ss;
		ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;
		return ss.str();
	}

	QI_EVENT_CLASS_TYPE(MouseMoved)
	QI_EVENT_CLASS_CATEGORY(eventCategoryMouse | eventCategoryInput)
private:
	float m_mouseX, m_mouseY;
};

class MouseScrolledEvent : public Event {
public:
	MouseScrolledEvent(const float xOffset, const float yOffset) : m_xOffset(xOffset), m_yOffset(yOffset) {}

	float getXOffset() const { return m_xOffset; }
	float getYOffset() const { return m_yOffset; }

	std::string toString() const override {
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
		return ss.str();
	}

	QI_EVENT_CLASS_TYPE(MouseScrolled)
	QI_EVENT_CLASS_CATEGORY(eventCategoryMouse | eventCategoryInput)
private:
	float m_xOffset, m_yOffset;
};

class MouseButtonEvent : public Event {
public:
	MouseCode getMouseButton() const { return m_button; }

	QI_EVENT_CLASS_CATEGORY(eventCategoryMouse | eventCategoryInput | eventCategoryMouseButton)
protected:
	MouseButtonEvent(const MouseCode button) : m_button(button) {}

	MouseCode m_button;
};

class MouseButtonPressedEvent : public MouseButtonEvent {
public:
	MouseButtonPressedEvent(const MouseCode button) : MouseButtonEvent(button) {}

	std::string toString() const override {
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_button;
		return ss.str();
	}

	QI_EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(const MouseCode button) : MouseButtonEvent(button) {}

	std::string toString() const override {
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << m_button;
		return ss.str();
	}

	QI_EVENT_CLASS_TYPE(MouseButtonReleased)
};

}
