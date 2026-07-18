#pragma once

#include <string>
#include <memory>
#include "core/io/MeshLoader.hpp"
#include "servers/rendering/RenderingServer.hpp"
#include "core/Assert.hpp"
#include "ResourceCache.hpp"
#include "renderer/vulkan/Mesh.hpp"

namespace Qi {

class ResourceLoader
{
public:
    ResourceLoader(RenderingServer& server) : m_renderingServer(server) {

    }

    template<typename T>
    std::shared_ptr<T> Load(const std::string& path) {
        QI_CORE_ASSERT(sizeof(T) == 0, "Resource type not supported");
    }

private:
    RenderingServer& m_renderingServer;
    ResourceCache<std::string, Mesh> m_meshCache;
    ResourceCache<std::string, Texture2D> m_textureCache;
    MeshLoader m_meshLoader;
};

template<> std::shared_ptr<Texture2D> ResourceLoader::Load<Texture2D>(const std::string& path);
template<> std::shared_ptr<Mesh> ResourceLoader::Load<Mesh>(const std::string& path);

}
