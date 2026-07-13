#pragma once

#include "events/Event.hpp"
#include <cstdint>
#include "core/Timestep.hpp"
#include "renderer/Renderer.hpp"
#include "core/Window.hpp"

namespace Qi {

class ImGuiManager {
public:
    ImGuiManager(Window& window, Renderer& renderer);
    ~ImGuiManager() = default;

    void init();
    void shutdown();

    void begin();
    void end();

    void render(Timestep ts);

    void onEvent(Event& event);

    void blockEvents(bool block) { m_blockEvents = block; }

    void setDarkThemeColors();

    uint32_t getActiveWidgetID() const;

private:
    bool m_blockEvents = true;
    Window& m_window;
    Renderer& m_renderer;
};

}
