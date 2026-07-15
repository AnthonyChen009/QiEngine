#include "Input.hpp"
#include "servers/input/InputServer.hpp"
#include "core/Application.hpp"

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

glm::vec2 getMousePosition() { return server().getMousePosition(); }
glm::vec2 getMouseDelta() { return server().getMouseDelta(); }

}
