#pragma once
#include <vector>
#include <utility>
#include <cstdint>
#include "types/Vertex.hpp"

namespace Qi {

using MeshData = std::pair<std::vector<Vertex>, std::vector<uint32_t>>;

class Primitives {
public:
    static MeshData generateBox(float width, float height, float depth);
    static MeshData generatePlane(float width, float depth);
    static MeshData generateQuad(float width, float height);
    static MeshData generateCylinder(float topRadius, float bottomRadius, float height, uint32_t radialSegments = 32);
    static MeshData generateSphere(float radius, uint32_t radialSegments = 32, uint32_t rings = 16);
    static MeshData generateCapsule(float radius, float height, uint32_t radialSegments = 32, uint32_t rings = 8);
    static MeshData generateTorus(float outerRadius, float innerRadius, uint32_t ringSegments = 32, uint32_t sides = 16);
    static MeshData generatePrism(float width, float height, float depth, float leftToRight = 0.5f);
};

}
