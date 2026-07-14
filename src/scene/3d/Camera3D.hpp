#pragma once
#include "scene/Node.hpp"
#include "scene/Components.hpp"
#include <glm/glm.hpp>
#include "CameraProperties.hpp"

namespace Qi {

class Camera3D : public Node {

public:
    Camera3D(const std::string& name = "Camera3D", const CameraProperties& props = {});

    void onReady() override;

    void setAspectRatio(float width, float height) { m_aspectRatio = width / height; }
    void updateProjection();
private:
    CameraProperties m_camProperties;
    float m_aspectRatio = 16.0f / 9.0f;

    glm::mat4 m_projection;
    glm::mat4 m_view;

};

}
