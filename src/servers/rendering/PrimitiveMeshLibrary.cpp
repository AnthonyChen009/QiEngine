#include "PrimitiveMeshLibrary.hpp"
#include "servers/rendering/RenderingServer.hpp"
#include "renderer/utils/Primitives.hpp"
#include "core/Assert.hpp"

namespace Qi {

std::shared_ptr<Mesh> PrimitiveMeshLibrary::getBox() {
    if (!m_box) {
        auto [vertices, indices] = Primitives::generateBox(1.0f, 1.0f, 1.0f);


        m_box = m_renderingServer.createMesh(vertices, indices);

    }
    return m_box;
}

std::shared_ptr<Mesh> PrimitiveMeshLibrary::getSphere() {
    if (!m_sphere) {
        auto [vertices, indices] = Primitives::generateSphere(1.0f, 32, 16);
        m_sphere = m_renderingServer.createMesh(vertices, indices);
    }
    return m_sphere;
}

std::shared_ptr<Mesh> PrimitiveMeshLibrary::getCapsule() {
    if (!m_capsule) {
        auto [vertices, indices] = Primitives::generateCapsule(0.5f, 1.0f, 32, 8);
        m_capsule = m_renderingServer.createMesh(vertices, indices);
    }
    return m_capsule;
}

std::shared_ptr<Mesh> PrimitiveMeshLibrary::getCylinder() {
    QI_CORE_ASSERT(false, "PrimitiveMeshLibrary::getCylinder is not implemented yet");
    return nullptr;
}

std::shared_ptr<Mesh> PrimitiveMeshLibrary::getCone() {
    QI_CORE_ASSERT(false, "PrimitiveMeshLibrary::getCone is not implemented yet");
    return nullptr;
}

std::shared_ptr<Mesh> PrimitiveMeshLibrary::getPlane() {
    QI_CORE_ASSERT(false, "PrimitiveMeshLibrary::getPlane is not implemented yet");
    return nullptr;
}

std::shared_ptr<Mesh> PrimitiveMeshLibrary::getQuad() {
    if (!m_quad) {
        auto [vertices, indices] = Primitives::generateQuad(1.0f, 1.0f);
        m_quad = m_renderingServer.createMesh(vertices, indices);
    }
    return m_quad;
}

std::shared_ptr<Mesh> PrimitiveMeshLibrary::getPrism() {
    QI_CORE_ASSERT(false, "PrimitiveMeshLibrary::getPrism is not implemented yet");
    return nullptr;
}

std::shared_ptr<Mesh> PrimitiveMeshLibrary::getTorus() {
    QI_CORE_ASSERT(false, "PrimitiveMeshLibrary::getTorus is not implemented yet");
    return nullptr;
}

}
