#include "servers/rendering/RenderingServer.hpp"
#include "renderer/vulkan/Texture2D.hpp"
#include "ResourceLoader.hpp"

namespace Qi {

template<>
std::shared_ptr<Texture2D>
ResourceLoader::Load<Texture2D>(const std::string& path) {
    // if (auto cached = m_textureCache.get(path)) {
    //     return cached;
    // }
    auto texture = m_renderingServer.createTexture2D(path);
    //m_textureCache.insert(path, texture);
    return texture;
}

template<>
std::shared_ptr<Mesh>
ResourceLoader::Load<Mesh>(const std::string& path) {
    if (auto cached = m_meshCache.get(path)) {
        return cached;
    }
    std::shared_ptr<Mesh> mesh = m_renderingServer.createMesh(path);
    m_meshCache.insert(path, mesh);
    return mesh;
}

}
