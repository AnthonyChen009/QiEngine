#pragma once
#include <glm/glm.hpp>
#include <string>
#include "renderer/vulkan/Texture2D.hpp"

namespace Qi {

enum class RotationEditMode {
    Euler,
    Quaternion,
    Basis
};

enum class EulerOrder {
    Euler,
    Quaternion,
    Basis
};

struct TagComponent {
    std::string tag;
};

struct TransformComponent {
    glm::vec2 position = { 0.0f, 0.0f };
    glm::vec2 size = { 1.0f, 1.0f };
    float rotation = 0.0f;

    glm::vec2 worldPosition = { 0.0f, 0.0f };
    float worldRotation = 0.0f;
};

struct RigidbodyComponent {
    glm::vec2 velocity = { 0.0f, 0.0f };
};

struct SpriteComponent {
    glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    Texture2D* texture = nullptr;
};

struct Transform3DComponent {
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {0.0f, 0.0f, 0.0f};

};

}
