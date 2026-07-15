#include "Camera3D.hpp"
#include "core/Timestep.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_relational.hpp>
#include <glm/trigonometric.hpp>
#include "events/ApplicationEvent.hpp"
#include "scene/3d/CameraProperties.hpp"
#include "scene/Components.hpp"

namespace Qi {

Camera3D::Camera3D(const std::string& name, const CameraProperties& props) : m_camProperties(props) {

}

void Camera3D::onReady() {
    Node3D::onReady();
    addComponent<Camera3DComponent>().properties = m_camProperties;
    updateProjection();
}

void Camera3D::onUpdate(Timestep ts) {
    if (!getComponent<Camera3DComponent>().isDirty) {
        if (glm::any(glm::notEqual(m_lastTransform, getWorldTransform()))) {
            getComponent<Camera3DComponent>().isDirty = true;
        }
    }
}

void Camera3D::setViewportSize(uint32_t width, uint32_t height) {
    getComponent<Camera3DComponent>().viewportWidth = static_cast<float>(width);
    getComponent<Camera3DComponent>().viewportHeight = static_cast<float>(height);
    getComponent<Camera3DComponent>().aspectRatio = static_cast<float>(width) / height;
    updateProjection();
}

void Camera3D::onEvent(Event& event) {
    if (event.getEventType() == EventType::WindowResize) {
        WindowResizeEvent& e = static_cast<WindowResizeEvent&>(event);
        setViewportSize(e.getWidth(), e.getHeight());
    }
}

void Camera3D::updateProjection() {
    Camera3DComponent& camera = getComponent<Camera3DComponent>();

    camera.projection = glm::perspectiveFov(
        glm::radians(camera.properties.fov),
        camera.viewportWidth,
        camera.viewportHeight,
        camera.properties.nearClip,
        camera.properties.farClip
    );
}

void Camera3D::updateView() {
    const glm::mat4& transform = getWorldTransform();

    getComponent<Camera3DComponent>().view = glm::inverse(transform);
    m_lastTransform = transform;
    getComponent<Camera3DComponent>().isDirty = false;
}

const glm::mat4& Camera3D::getViewMatrix() {
    if (getComponent<Camera3DComponent>().isDirty) {
        updateView();
    }

    return getComponent<Camera3DComponent>().view;
}

void Camera3D::setNearClip(float nearClip) {
    getComponent<Camera3DComponent>().properties.nearClip = nearClip;
    updateProjection();
}

void Camera3D::setFarClip(float farClip) {
    getComponent<Camera3DComponent>().properties.farClip = farClip;
    updateProjection();
}

void Camera3D::setOrthoSize(float size) {
    getComponent<Camera3DComponent>().properties.orthoSize = size;
    updateProjection();
}

void Camera3D::setFOV(float fovDegs) {
    getComponent<Camera3DComponent>().properties.fov = fovDegs;
    updateProjection();
}

void Camera3D::setProjectionType(CameraProperties::ProjectionType type) {
    getComponent<Camera3DComponent>().properties.projection = type;
    updateProjection();
}

float Camera3D::getNearClip() {
    return getComponent<Camera3DComponent>().properties.nearClip;
}

float Camera3D::getFarClip() {
    return getComponent<Camera3DComponent>().properties.farClip;
}

float Camera3D::getOrthoSize() {
    return getComponent<Camera3DComponent>().properties.orthoSize;
}

float Camera3D::getFOV() {
    return getComponent<Camera3DComponent>().properties.fov;
}

CameraProperties::ProjectionType Camera3D::getProjectionType() {
    return getComponent<Camera3DComponent>().properties.projection;
}

const glm::mat4& Camera3D::getProjectionMatrix() const {
    return getComponent<Camera3DComponent>().projection;
}

glm::mat4 Camera3D::getViewProjectionMatrix() {
    if (getComponent<Camera3DComponent>().isDirty)
        updateView();

    return getComponent<Camera3DComponent>().projection * getComponent<Camera3DComponent>().view;
}

}
