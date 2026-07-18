#include "CameraController.hpp"
#include "core/Log.hpp"
#include "core/MouseCodes.hpp"
#include "core/Timestep.hpp"
#include "core/api/Input.hpp"
#include "events/Event.hpp"
#include "events/MouseEvent.hpp"
#include "scene/3d/Camera3D.hpp"
#include "scene/Components.hpp"
#include <glm/ext/vector_float2.hpp>

CameraController::CameraController(const std::string& name, const Qi::CameraProperties& props) : Camera3D(name, props) {

}

void CameraController::onReady() {
    Qi::Camera3D::onReady();
}

void CameraController::onUpdate(Qi::Timestep ts) {
    Qi::Camera3D::onUpdate(ts);

    float velocity = 5.0f * ts.getSeconds();
    glm::quat rotation = getRotation();

    glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // world up, keeps vertical movement independent of pitch

    glm::vec3 position = getPosition();

    if (Qi::Input::isKeyPressed(Qi::Key::W))
        position += forward * velocity;
    if (Qi::Input::isKeyPressed(Qi::Key::S))
        position -= forward * velocity;
    if (Qi::Input::isKeyPressed(Qi::Key::A))
        position -= right * velocity;
    if (Qi::Input::isKeyPressed(Qi::Key::D))
        position += right * velocity;
    if (Qi::Input::isKeyPressed(Qi::Key::Space))
        position += up * velocity;
    if (Qi::Input::isKeyPressed(Qi::Key::LeftControl))
        position -= up * velocity;

    setPosition(position);
}

void CameraController::onEvent(Qi::Event& event) {
    Qi::Camera3D::onEvent(event);

    if (event.getEventType() == Qi::EventType::MouseButtonReleased) {
        Qi::MouseButtonReleasedEvent& e = static_cast<Qi::MouseButtonReleasedEvent&>(event);
        if (e.getMouseButton() == Qi::Mouse::ButtonRight) {
            Qi::Input::setCursorMode(Qi::CursorMode::Normal);
        }
    }

    if (event.getEventType() == Qi::EventType::MouseButtonPressed) {
        Qi::MouseButtonPressedEvent& e = static_cast<Qi::MouseButtonPressedEvent&>(event);
        if (e.getMouseButton() == Qi::Mouse::ButtonRight) {
            Qi::Input::setCursorMode(Qi::CursorMode::Locked);
        }
    }

    if (event.getEventType() == Qi::EventType::MouseMoved) {
        Qi::MouseMovedEvent& e = static_cast<Qi::MouseMovedEvent&>(event);
        glm::vec2 mousePos = {e.getX(), e.getY()};
        glm::vec2 delta = (mousePos - m_lastMousePos) * m_sensitivity;
        m_lastMousePos = mousePos;

        if (Qi::Input::isMouseButtonPressed(Qi::Mouse::ButtonRight)) {
            if (delta.x != 0.0f || delta.y != 0.0f) {
                m_yaw -= delta.x * 0.3f;
                m_pitch -= delta.y * 0.3f;
                m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

                glm::quat yawQuat = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 1, 0));
                glm::quat pitchQuat = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));

                setRotation(glm::normalize(yawQuat * pitchQuat));
            }
        }
    }
}
