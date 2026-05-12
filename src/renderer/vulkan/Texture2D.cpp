#include "Texture2D.hpp"
#include "renderer/Renderer2D.hpp"

namespace Qi {

Texture2D* Texture2D::load(const std::string& path) {
    return Renderer2D::loadTexture(path);
}

}
