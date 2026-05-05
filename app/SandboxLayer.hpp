#pragma once
#include "QiEngine.hpp"


class SandboxLayer : public Qi::Layer {
    public:
	SandboxLayer();
	virtual ~SandboxLayer() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Qi::Timestep ts) override;
	//virtual void OnImGuiRender() override;
	void onEvent(Qi::Event& e) override;
};
