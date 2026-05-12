#pragma once

#include "core/Layer.hpp"

#include "events/ApplicationEvent.hpp"
#include "events/KeyEvent.hpp"
#include "events/MouseEvent.hpp"

namespace Qi {

class ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onEvent(Event& e) override;

	void begin();
	void end();

	void blockEvents(bool block) { m_blockEvents = block; }
	virtual void onImGuiRender() override;
	void setDarkThemeColors();

	uint32_t getActiveWidgetID() const;
private:
	bool m_blockEvents = true;
};

}
