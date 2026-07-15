#pragma once

#include "core/Base.hpp"
#include "renderer/vulkan/Mesh.hpp"
namespace Qi::MeshPrimitives {
    Ref<Mesh> BoxMesh();
    Ref<Mesh> SphereMesh();
    Ref<Mesh> CapsuleMesh();
    Ref<Mesh> QuadMesh();
}
