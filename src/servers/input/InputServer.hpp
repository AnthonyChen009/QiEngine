#pragma once
#include <array>
#include <unordered_set>
#include "core/KeyCodes.hpp"
#include "core/MouseCodes.hpp"
#include "glm/vec2.hpp"
namespace Qi {

class Event;
class KeyPressedEvent;
class KeyReleasedEvent;
class MouseButtonPressedEvent;
class MouseButtonReleasedEvent;
class MouseMovedEvent;
class MouseScrolledEvent;

class InputServer {
public:
    InputServer() = default;

    bool isKeyPressed(KeyCode key);
    bool isKeyJustPressed(KeyCode key);
    bool isKeyJustReleased(KeyCode key);

    bool isMouseButtonPressed(MouseCode button);
    bool isMouseButtonJustPressed(MouseCode button);
    bool isMouseButtonJustReleased(MouseCode button);

    glm::vec2 getMousePosition() const { return { m_mouseX, m_mouseY }; }
    glm::vec2 getMouseDelta() const { return { m_mouseX - m_previousMouseX, m_mouseY - m_previousMouseY }; }

    void onEvent(Event& event);

    void update();

private:
    bool onKeyPressed(KeyPressedEvent& e);
    bool onKeyReleased(KeyReleasedEvent& e);
    bool onMouseButtonPressed(MouseButtonPressedEvent& e);
    bool onMouseButtonReleased(MouseButtonReleasedEvent& e);
    bool onMouseMoved(MouseMovedEvent& e);

    std::array<bool, 512> m_currentKeys{};
    std::unordered_set<KeyCode> m_justPressedKeys;
    std::unordered_set<KeyCode> m_justReleasedKeys;

    std::array<bool, 8> m_currentMouseButtons{};
    std::unordered_set<MouseCode> m_justPressedMouseButtons;
    std::unordered_set<MouseCode> m_justReleasedMouseButtons;

    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;
    float m_previousMouseX = 0.0f;
    float m_previousMouseY = 0.0f;
};

}
