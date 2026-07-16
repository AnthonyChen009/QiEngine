#pragma once

#include "events/Event.hpp"
#include <cstdint>
#include "core/Timestep.hpp"
#include "renderer/Renderer.hpp"
#include "core/Window.hpp"

namespace Qi {

class ImGuiLayer {
public:
    ImGuiLayer(Window& window, Renderer& renderer);
    ~ImGuiLayer() = default;

    void init();
    void shutdown();

    void begin();
    void end();

    void render(Timestep ts);

    void onEvent(Event& event);

    void blockEvents(bool block) { m_blockEvents = block; }

    void setDarkThemeColors();

    uint32_t getActiveWidgetID() const;

    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }
    void toggleVisible() { m_visible = !m_visible; }

private:
    bool m_blockEvents = true;
    Window& m_window;
    Renderer& m_renderer;
    bool m_visible = true;
};

}
