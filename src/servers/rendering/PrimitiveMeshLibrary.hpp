#pragma once
#include <memory>
#include "renderer/vulkan/Mesh.hpp"

namespace Qi {

class RenderingServer;

class PrimitiveMeshLibrary {
public:
    explicit PrimitiveMeshLibrary(RenderingServer& server) : m_renderingServer(server) {}

    std::shared_ptr<Mesh> getBox();
    std::shared_ptr<Mesh> getSphere();
    std::shared_ptr<Mesh> getCapsule();
    std::shared_ptr<Mesh> getCylinder();
    std::shared_ptr<Mesh> getCone();
    std::shared_ptr<Mesh> getPlane();
    std::shared_ptr<Mesh> getQuad();
    std::shared_ptr<Mesh> getPrism();
    std::shared_ptr<Mesh> getTorus();

private:
    RenderingServer& m_renderingServer;

    std::shared_ptr<Mesh> m_box;
    std::shared_ptr<Mesh> m_sphere;
    std::shared_ptr<Mesh> m_capsule;
    std::shared_ptr<Mesh> m_cylinder;
    std::shared_ptr<Mesh> m_cone;
    std::shared_ptr<Mesh> m_plane;
    std::shared_ptr<Mesh> m_quad;
    std::shared_ptr<Mesh> m_prism;
    std::shared_ptr<Mesh> m_torus;
};

}
