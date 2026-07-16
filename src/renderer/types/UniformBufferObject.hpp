#pragma once
#include <glm/glm.hpp>

namespace Qi {

struct UniformBufferObject {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;

    alignas(16) glm::vec3 lightDirection;
    alignas(16) glm::vec3 lightColor;
    float lightIntensity;

    alignas(16) glm::vec3 ambientColor;
    float ambientIntensity;
};

}
