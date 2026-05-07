#pragma once
#include "QiEngine.hpp"
#include "glm/glm.hpp"
#include <glm/ext/vector_float2.hpp>

struct Quad {
    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 size = glm::vec2(50.0f, 50.0f);
    float rotation = 0.0f;
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec2 velocity = glm::vec2(300.0f, 200.0f);
};

class SandboxLayer : public Qi::Layer {
public:
	SandboxLayer();
	virtual ~SandboxLayer() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Qi::Timestep ts) override;
	//virtual void OnImGuiRender() override;
	void onEvent(Qi::Event& e) override;
private:
    Quad m_quad;
};
