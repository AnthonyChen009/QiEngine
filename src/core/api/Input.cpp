#include "Input.hpp"
#include "servers/input/InputServer.hpp"
#include "core/Application.hpp"
#include <GLFW/glfw3.h>

namespace Qi::Input {

InputServer& server() {
    return Application::get().getInputServer();
}

bool isKeyPressed(KeyCode key) { return server().isKeyPressed(key); }
bool isKeyJustPressed(KeyCode key) { return server().isKeyJustPressed(key); }
bool isKeyJustReleased(KeyCode key) { return server().isKeyJustReleased(key); }

bool isMouseButtonPressed(MouseCode button) { return server().isMouseButtonPressed(button); }
bool isMouseButtonJustPressed(MouseCode button) { return server().isMouseButtonJustPressed(button); }
bool isMouseButtonJustReleased(MouseCode button) { return server().isMouseButtonJustReleased(button); }

void setCursorMode(CursorMode mode) {
    int glfwMode = GLFW_CURSOR_NORMAL;
        switch (mode) {
        case CursorMode::Normal: glfwMode = GLFW_CURSOR_NORMAL;   break;
        case CursorMode::Hidden: glfwMode = GLFW_CURSOR_HIDDEN;   break;
        case CursorMode::Locked: glfwMode = GLFW_CURSOR_DISABLED; break;
    }
    glfwSetInputMode(static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow()), GLFW_CURSOR, glfwMode);
}

glm::vec2 getMousePosition() { return server().getMousePosition(); }
glm::vec2 getMouseDelta() { return server().getMouseDelta(); }

}
