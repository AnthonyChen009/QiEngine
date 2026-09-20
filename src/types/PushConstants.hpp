#pragma once
#include "glm/glm.hpp"

struct PushConstant {
    glm::mat4 transform;
    glm::vec4 color;
    uint32_t textureIndex;
    uint32_t useRt = 0;
};
