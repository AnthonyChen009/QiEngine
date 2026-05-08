#pragma once
#include "glm/glm.hpp"

struct PushConstant2D {
    glm::mat4 transform;
    glm::vec4 color;
};
