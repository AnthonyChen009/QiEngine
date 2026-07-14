#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include "renderer/vulkan/Texture2D.hpp"
#include <glm/gtc/quaternion.hpp>

namespace Qi {

enum class RotationEditMode {
    Euler,
    Quaternion,
    Basis
};

enum class EulerOrder {
    YXZ,
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
    glm::vec3 position {0.0f};
    glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale {1.0f};

    glm::mat4 localTransform {1.0f};
    glm::mat4 worldTransform {1.0f};
};

}
