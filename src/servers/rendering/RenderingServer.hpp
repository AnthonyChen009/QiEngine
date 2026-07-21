#pragma once

#include "core/Base.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Renderer2D.hpp"
#include "ImGuiLayer.hpp"
#include "renderer/Renderer3D.hpp"
#include "renderer/vulkan/Texture2D.hpp"
#include "scene/Scene.hpp"
#include <memory>
#include "renderer/vulkan/Mesh.hpp"
#include "PrimitiveMeshLibrary.hpp"
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
    std::shared_ptr<Mesh> createMesh(const std::string& path);
    std::shared_ptr<Mesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ImGuiLayer& getImGuiLayer();
    PrimitiveMeshLibrary& getPrimitives();
    void waitIdle();
private:
    void render2D(Scene& scene);
    void render3D(Scene& scene);
private:
    Scope<Renderer> m_renderer;
    Scope<Renderer2D> m_renderer2D;
    Scope<Renderer3D> m_renderer3D;
    Scope<ImGuiLayer> m_imGuiLayer;
    PrimitiveMeshLibrary m_primitives{*this};
    bool m_warnedNoCamera = false;
};

}
