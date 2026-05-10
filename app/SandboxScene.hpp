#pragma once
#include "QiEngine.hpp"

class SandboxScene : public Qi::Scene {
public:
    void onReady() override;
    void onUpdate(Qi::Timestep ts) override;
    void onEvent(Qi::Event& e) override;

private:
    Qi::Sprite2D* m_player = nullptr;
    Qi::Sprite2D* m_child = nullptr;
};
