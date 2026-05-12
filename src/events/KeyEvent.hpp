#pragma once

#include "events/Event.hpp"
#include "core/KeyCodes.hpp"

namespace Qi {

class KeyEvent : public Event {
public:
    KeyCode getKeyCode() const {
       return m_keyCode;
    }
protected:
	KeyEvent(const KeyCode keycode) : m_keyCode(keycode) {}
	KeyCode m_keyCode;
};

class KeyPressedEvent : public KeyEvent {
public:
    KeyPressedEvent(const KeyCode keycode, bool isRepeat = false) : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

	bool IsRepeat() const { return m_IsRepeat; }

	std::string toString() const override {
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_keyCode << " (repeat = " << m_IsRepeat << ")";
		return ss.str();
	}
	QI_EVENT_CLASS_TYPE(KeyPressed)
private:
    bool m_IsRepeat;
};

class KeyReleasedEvent : public KeyEvent {
public:
    KeyReleasedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

    std::string toString() const override {
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << m_keyCode;
		return ss.str();
	}
	QI_EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent : public KeyEvent {
public:
	KeyTypedEvent(const KeyCode keycode) : KeyEvent(keycode) {}
	std::string toString() const override {
		std::stringstream ss;
		ss << "KeyTypedEvent: " << m_keyCode;
		return ss.str();
	}

	QI_EVENT_CLASS_TYPE(KeyTyped)
};

}
