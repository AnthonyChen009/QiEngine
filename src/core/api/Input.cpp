#include "Input.hpp"
#include "servers/input/InputServer.hpp"
#include "core/Application.hpp"
#include <SDL3/SDL.h>

namespace Qi::Input {

static float s_savedCursorX = 0.0f;
static float s_savedCursorY = 0.0f;
static CursorMode s_currentCursorMode = CursorMode::Normal;

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
    SDL_Window* window = static_cast<SDL_Window*>(Application::get().getWindow().getNativeWindow());

    if (mode == CursorMode::Locked && s_currentCursorMode != CursorMode::Locked) {
        // Save position before locking
        SDL_GetMouseState(&s_savedCursorX, &s_savedCursorY);
    }

    switch (mode) {
        case CursorMode::Normal:
            SDL_SetWindowRelativeMouseMode(window, false);
            SDL_ShowCursor();
            if (s_currentCursorMode == CursorMode::Locked) {
                SDL_WarpMouseInWindow(window, s_savedCursorX, s_savedCursorY);
            }
            break;
        case CursorMode::Hidden:
            SDL_SetWindowRelativeMouseMode(window, false);
            SDL_HideCursor();
            break;
        case CursorMode::Locked:
            SDL_SetWindowRelativeMouseMode(window, true);
            break;
    }

    s_currentCursorMode = mode;
}

glm::vec2 getMousePosition() { return server().getMousePosition(); }
glm::vec2 getMouseDelta() { return server().getMouseDelta(); }

}
