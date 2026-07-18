#pragma once

namespace Qi {
struct CameraProperties
{
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };

    ProjectionType projection = ProjectionType::Perspective;

    float fov = 45.0f;

    float nearClip = 0.1f;
    float farClip = 1000.0f;

    float orthoSize = 10.0f;
};
}
