#pragma once
#include "glm/glm.hpp"

namespace Qi {
struct GPUMaterial {
    glm::vec3 albedo;
    float roughness;
    float metallic;
    glm::vec3 emissionColor;
    float emissionPower;
    float specularProbability;
    uint32_t isGlass;
    float ior;
    glm::vec3 absorption;
    float absorptionStrength;
};
}
