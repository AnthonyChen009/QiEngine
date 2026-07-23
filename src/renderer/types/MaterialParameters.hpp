#pragma once
#include <glm/glm.hpp>

namespace Qi {
struct MaterialParameters {
    glm::vec3 albedo = glm::vec3(1.0f);
    float roughness = 0.5f;
    float metallic = 0.0f;
    glm::vec3 emissionColor = glm::vec3(0.0f);
    float emissionPower = 0.0f;
    float specularProbability = 0.0f;
    bool isGlass = false;
    float ior = 1.0f;
    glm::vec3 absorption = glm::vec3(0.0f);
    float absorptionStrength = 0.0f;
};
}
