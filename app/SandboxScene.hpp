#pragma once
#include "QiEngine.hpp"
#include "game/CameraController.hpp"
#include "scene/2d/Sprite2D.hpp"
#include "scene/3d/Camera3D.hpp"
#include "scene/3d/MeshInstance3D.hpp"
#include "scene/3d/Node3D.hpp"

class SandboxScene : public Qi::Scene {
public:
    void onReady() override;
    void onUpdate(Qi::Timestep ts) override;
    void onEvent(Qi::Event& event) override;

private:
    Qi::MeshInstance3D* m_testNode;
    Qi::MeshInstance3D* m_testNode2;
    Qi::Sprite2D* m_player;
    CameraController* m_camera;
    Qi::Node3D* m_directionLight;

    float m_turntableAngle = 0.0f;
};
