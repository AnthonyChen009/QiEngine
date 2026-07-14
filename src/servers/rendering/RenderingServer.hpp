#pragma once

#include "core/Base.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Renderer2D.hpp"
#include "managers/imgui/ImGuiManager.hpp"
#include "renderer/vulkan/Texture2D.hpp"
#include "scene/Scene.hpp"
#include <memory>
namespace Qi {

class RenderingServer {

public:
    RenderingServer(Window& window, GraphicsAPI graphicsAPI);
    ~RenderingServer();
    bool beginFrame();
    void endFrame();
    void render(Scene& scene);
    void beginImGui();
    void endImGui();
    void onWindowResize(unsigned int x, unsigned int y);
    void setVSync(bool enabled);
    void onEvent(Qi::Event& event);
    std::shared_ptr<Texture2D> createTexture2D(const std::string& path);
    ImGuiManager& getImGuiManager();
private:
    void render2D();
    void render3D();
private:
    Scope<Renderer> m_renderer;
    Scope<Renderer2D> m_renderer2D;
    Scope<ImGuiManager> m_imGuiManager;
};

}
