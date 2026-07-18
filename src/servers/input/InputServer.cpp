#include "InputServer.hpp"
#include "core/Log.hpp"
#include "events/KeyEvent.hpp"
#include "events/MouseEvent.hpp"

namespace Qi {

bool InputServer::isKeyPressed(KeyCode key) {
    return m_currentKeys.contains(key);
}


bool InputServer::isKeyJustPressed(KeyCode key) {
    return m_justPressedKeys.contains(key);
}

bool InputServer::isKeyJustReleased(KeyCode key) {
    return m_justReleasedKeys.contains(key);
}

bool InputServer::isMouseButtonPressed(MouseCode button) {
    return m_currentMouseButtons.contains(button);
}

bool InputServer::isMouseButtonJustPressed(MouseCode button) {
    return m_justPressedMouseButtons.contains(button);
}

bool InputServer::isMouseButtonJustReleased(MouseCode button) {
    return m_justReleasedMouseButtons.contains(button);
}

void InputServer::onEvent(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<KeyPressedEvent>([this](KeyPressedEvent& e) { return onKeyPressed(e); });
    dispatcher.dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& e) { return onKeyReleased(e); });
    dispatcher.dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) { return onMouseButtonPressed(e); });
    dispatcher.dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& e) { return onMouseButtonReleased(e); });
    dispatcher.dispatch<MouseMovedEvent>([this](MouseMovedEvent& e) { return onMouseMoved(e); });
}

bool InputServer::onKeyPressed(KeyPressedEvent& event) {
    KeyCode key = event.getKeyCode();
    m_currentKeys.insert(key);
    if (!event.IsRepeat()) {
        m_justPressedKeys.insert(key);
    }
    return false;
}


bool InputServer::onKeyReleased(KeyReleasedEvent& event) {
    KeyCode key = event.getKeyCode();
    m_currentKeys.erase(key);
    m_justReleasedKeys.insert(key);
    return false;
}

bool InputServer::onMouseButtonPressed(MouseButtonPressedEvent& event) {
    MouseCode button = event.getMouseButton();
    m_currentMouseButtons.insert(button);
    m_justPressedMouseButtons.insert(button);
    return false;
}

bool InputServer::onMouseButtonReleased(MouseButtonReleasedEvent& event) {
    MouseCode button = event.getMouseButton();
    m_currentMouseButtons.erase(button);
    m_justReleasedMouseButtons.insert(button);
    return false;
}

bool InputServer::onMouseMoved(MouseMovedEvent& event) {
    m_mouseX = event.getX();
    m_mouseY = event.getY();
    return false;
}

void InputServer::update() {
    m_justPressedKeys.clear();
    m_justReleasedKeys.clear();
    m_justPressedMouseButtons.clear();
    m_justReleasedMouseButtons.clear();

    m_previousMouseX = m_mouseX;
    m_previousMouseY = m_mouseY;
}

}
