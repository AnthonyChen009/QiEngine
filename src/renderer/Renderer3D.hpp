#pragma once

#include "core/Window.hpp"
#include "glm/glm.hpp"
#include "renderer/types/MaterialParameters.hpp"
#include "vulkan/Texture2D.hpp"
#include "Renderer.hpp"
#include "renderer/vulkan/Mesh.hpp"

namespace Qi {

class Renderer3D {
public:
    Renderer3D(Renderer& render);
    void beginScene();
    void endScene();
    void drawCube();
    void drawMesh(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform, std::shared_ptr<Texture2D>& texture, bool useRT);
    std::shared_ptr<Material> createMaterial(const MaterialParameters& params, const std::string& path);

private:
    Renderer& m_renderer;
};

}
