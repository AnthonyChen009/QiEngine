#pragma once
#include "scene/3d/Node3D.hpp"
#include "scene/Node.hpp"
#include "scene/Components.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include "CameraProperties.hpp"

namespace Qi {

class Camera3D : public Node3D {

public:
    Camera3D(const std::string& name = "Camera3D", const CameraProperties& props = {});

    void onReady() override;
    void onUpdate(Timestep ts) override;
    void onEvent(Event& event) override;

    void setViewportSize(uint32_t width, uint32_t height);

    void setNearClip(float nearClip);
    void setFarClip(float farClip);
    void setOrthoSize(float size);
    void setFOV(float fovDegs);
    void setProjectionType(CameraProperties::ProjectionType type);

    float getNearClip();
    float getFarClip();
    float getOrthoSize();
    float getFOV();
    CameraProperties::ProjectionType getProjectionType();

    const glm::mat4& getProjectionMatrix() const;
    const glm::mat4& getViewMatrix();
    glm::mat4 getViewProjectionMatrix();

private:
    void updateProjection();
    void updateView();

private:
    CameraProperties m_camProperties = {};
    glm::mat4 m_lastTransform{};


};

}
