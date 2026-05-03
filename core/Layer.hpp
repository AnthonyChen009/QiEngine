#pragma once

#include "Base.hpp"
#include "Timestep.hpp"
#include "events/Event.hpp"

namespace Qi {

class Layer {
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer() = default;

	virtual void onAttach() {}
	virtual void onDetach() {}
	virtual void onUpdate(Timestep ts) {}
	virtual void onImGuiRender() {}
	virtual void onEvent(Event& event) {}

	const std::string& getName() const { return m_debugName; }
protected:
	std::string m_debugName;
};

}
