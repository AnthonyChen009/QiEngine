#pragma once

#include <string>
#include <memory>
#include "servers/rendering/RenderingServer.hpp"
#include "core/Assert.hpp"

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
};

}

#include "servers/rendering/RenderingServer.hpp"
#include "renderer/vulkan/Texture2D.hpp"

namespace Qi {

template<>
inline std::shared_ptr<Texture2D>
ResourceLoader::Load<Texture2D>(const std::string& path)
{
    return m_renderingServer.createTexture2D(path);
}

}
