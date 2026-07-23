#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include "renderer/Material.hpp"
#include "renderer/vulkan/Texture2D.hpp"
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include "renderer/vulkan/Mesh.hpp"
#include "3d/CameraProperties.hpp"

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

struct Rigidbody2DComponent {
    glm::vec2 velocity = { 0.0f, 0.0f };
};

struct Rigidbody3DComponent {
    glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };
};

struct SpriteComponent {
    glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    std::shared_ptr<Texture2D> texture = nullptr;
};

struct Transform3DComponent {
    glm::vec3 position {0.0f};
    glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale {1.0f};

    glm::mat4 localTransform {1.0f};
    glm::mat4 worldTransform {1.0f};

    void updateLocalTransform() {
        localTransform =
            glm::translate(glm::mat4(1.0f), position) *
            glm::mat4_cast(rotation) *
            glm::scale(glm::mat4(1.0f), scale);
    }
    bool isDirty = true;
};

struct Camera3DComponent {
    CameraProperties properties;
    float aspectRatio = 16.0f / 9.0f;
    glm::mat4 projection{ 1.0f };
    glm::mat4 view{ 1.0f };
    glm::mat4 inverseProjection{ 1.0f };
	glm::mat4 inverseView{ 1.0f };
	glm::vec3 forwardDirection{0.0f, 0.0f, 0.0f};
	bool isDirty = true;
	float viewportWidth = 1280.0f;
    float viewportHeight = 720.0f;
};

struct DirectionalLightComponent {
    glm::vec3 direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.5f));
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.95f);
    float intensity = 1.0f;
};

struct MeshComponent {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture2D> albedoTexture = nullptr;
    std::shared_ptr<Material> material = nullptr;
};

}
