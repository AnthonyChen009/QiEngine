#include "MeshPrimitives.hpp"
#include "servers/input/InputServer.hpp"
#include "core/Application.hpp"
#include "servers/rendering/PrimitiveMeshLibrary.hpp"
#include "servers/rendering/RenderingServer.hpp"

namespace Qi::MeshPrimitives {

PrimitiveMeshLibrary& primitives() {
    return Application::get().getRenderingServer().getPrimitives();
}

Ref<Mesh> BoxMesh() {
    return primitives().getBox();
}

Ref<Mesh> SphereMesh() {
    return primitives().getSphere();
}

Ref<Mesh> CapsuleMesh() {
    return primitives().getCapsule();
}

Ref<Mesh> QuadMesh() {
    return primitives().getQuad();
}

}
