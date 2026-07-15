#pragma once

#include "core/KeyCodes.hpp"
#include "core/MouseCodes.hpp"
#include <glm/vec2.hpp>

namespace Qi::Input {
    bool isKeyPressed(KeyCode key);
    bool isKeyJustReleased(KeyCode key);

    bool isKeyJustPressed(KeyCode key);

    bool isMouseButtonPressed(MouseCode button);
    bool isMouseButtonJustReleased(MouseCode button);

    bool isMouseButtonJustPressed(MouseCode button);

    glm::vec2 getMousePosition();
    glm::vec2 getMouseDelta();
}
