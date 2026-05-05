#pragma once

#include "Assert.hpp"
#include "Layer.hpp"

#include <vector>

namespace Qi {

class LayerStack {
public:
	LayerStack();
	~LayerStack();

	void pushLayer(Layer* layer);
	void pushOverlay(Layer* overlay);
	void popLayer(Layer* layer);
	void popOverlay(Layer* overlay);

	Layer* operator[](size_t index) {
		QI_CORE_ASSERT(index >= 0 && index < m_layers.size());
		return m_layers[index];
	}

	const Layer* operator[](size_t index) const {
		QI_CORE_ASSERT(index >= 0 && index < m_layers.size());
		return m_layers[index];
	}

	size_t size() const { return m_layers.size(); }

	std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_layers.end(); }
private:
	std::vector<Layer*> m_layers;
	unsigned int m_layerInsertIndex = 0;
};

}
