#pragma once
#include <vector>
#include <utility>
#include <cstdint>
#include "types/Vertex.hpp"

namespace Qi {

using MeshDataPair = std::pair<std::vector<Vertex>, std::vector<uint32_t>>;

class Primitives {
public:
    static MeshDataPair generateBox(float width, float height, float depth);
    static MeshDataPair generatePlane(float width, float depth);
    static MeshDataPair generateQuad(float width, float height);
    static MeshDataPair generateCylinder(float topRadius, float bottomRadius, float height, uint32_t radialSegments = 32);
    static MeshDataPair generateSphere(float radius, uint32_t radialSegments = 32, uint32_t rings = 16);
    static MeshDataPair generateCapsule(float radius, float height, uint32_t radialSegments = 32, uint32_t rings = 8);
    static MeshDataPair generateTorus(float outerRadius, float innerRadius, uint32_t ringSegments = 32, uint32_t sides = 16);
    static MeshDataPair generatePrism(float width, float height, float depth, float leftToRight = 0.5f);
};

}
