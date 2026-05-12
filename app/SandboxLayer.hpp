#pragma once
#include "QiEngine.hpp"
#include "glm/glm.hpp"
#include <glm/ext/vector_float2.hpp>
#include "SandboxScene.hpp"

class SandboxLayer : public Qi::Layer {
public:
	SandboxLayer();
	virtual ~SandboxLayer() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Qi::Timestep ts) override;
	void onImGuiRender() override;
	void onEvent(Qi::Event& e) override;
private:
    float m_timeStep = 0;
    SandboxScene* m_scene = nullptr;
};
