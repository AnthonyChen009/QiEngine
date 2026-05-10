#pragma once
#include <glm/glm.hpp>
#include <string>
#include "vulkan/Texture2D.hpp"

namespace Qi {

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

}
