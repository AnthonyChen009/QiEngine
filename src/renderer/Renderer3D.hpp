#pragma once

#include "core/Window.hpp"
#include "glm/glm.hpp"
#include "vulkan/Texture2D.hpp"
#include "Renderer.hpp"

namespace Qi {

class Renderer3D {
public:
    Renderer3D(Renderer& render);
    void beginScene();
    void endScene();
    void drawCube();


private:
    Renderer& m_renderer;
};

}
