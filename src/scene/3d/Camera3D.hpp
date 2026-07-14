#pragma once
#include "scene/Node.hpp"
#include "scene/Components.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include "CameraProperties.hpp"

namespace Qi {

class Camera3D : public Node {

public:
    Camera3D(const std::string& name = "Camera3D", const CameraProperties& props = {});

    void onReady() override;
    void onUpdate(Timestep ts) override;

    void setViewportSize(uint32_t width, uint32_t height) {
        m_aspectRatio = static_cast<float>(width) / height;
        updateProjection();
    }

    void updateProjection();
    void updateView();

    const glm::mat4& getProjectionMatrix() const { return m_projection; }
    const glm::mat4& getViewMatrix();
    glm::mat4 getViewProjectionMatrix() const { return m_projection * m_view; }

    const glm::mat4& getWorldTransform() const;



private:
    CameraProperties m_camProperties;
    float m_aspectRatio = 16.0f / 9.0f;

    glm::mat4 m_projection;
    glm::mat4 m_view;

    glm::mat4 m_lastTransform{};

    bool m_isDirty = true;


};

}
