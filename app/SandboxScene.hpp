#pragma once

#include "scene/Scene.hpp"
#include "scene/Components.hpp"

class SandboxScene : public Qi::Scene {
public:
    SandboxScene();
    void onUpdate(Qi::Timestep ts) override;
    void onEvent(Qi::Event& e) override;

private:
    Qi::Entity m_quad;
};
