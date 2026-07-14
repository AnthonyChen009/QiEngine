#include "Camera3D.hpp"
#include "core/Timestep.hpp"
#include "scene/Components.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>

namespace Qi {

Camera3D::Camera3D(const std::string& name, const CameraProperties& props) : m_camProperties(props) {

}

void Camera3D::onReady() {
    addComponent<Transform3DComponent>();
}

void Camera3D::onUpdate(Timestep ts) {
    if (!m_isDirty) {
        if (m_lastTransform != getWorldTransform()) {
            m_isDirty = true;
        }
    }
}

const glm::mat4& Camera3D::getWorldTransform() const {
    return getComponent<Transform3DComponent>().worldTransform;
}

void Camera3D::updateView() {
    Transform3DComponent& transform = getComponent<Transform3DComponent>();
    m_view = glm::inverse(transform.worldTransform);
    m_lastTransform = transform.worldTransform;
    m_isDirty = false;
}

const glm::mat4& Camera3D::getViewMatrix() {
    if (m_isDirty) {
        updateView();
    }

    return m_view;
}

}
