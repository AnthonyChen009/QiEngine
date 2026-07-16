#pragma once

#include "core/Timestep.hpp"
#include "events/Event.hpp"
#include "scene/3d/Camera3D.hpp"
#include "scene/3d/CameraProperties.hpp"
#include <glm/ext/vector_float2.hpp>

class CameraController : public Qi::Camera3D {
public:
    CameraController(const std::string& name = "Camera3D", const Qi::CameraProperties& props = {});
    void onReady() override;
    void onUpdate(Qi::Timestep ts) override;
    void onEvent(Qi::Event& event) override;

private:
    glm::vec2 m_lastMousePos{};
    float m_sensitivity = 0.1f;
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;

};
