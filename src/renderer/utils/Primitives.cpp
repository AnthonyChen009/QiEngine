#include "Primitives.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/glm.hpp>
#include "core/Assert.hpp"

namespace Qi {

static constexpr float PI = 3.14159265358979323846f;
static constexpr glm::vec3 kDefaultColor = { 1.0f, 1.0f, 1.0f };

MeshData Primitives::generateBox(float width, float height, float depth) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float hw = width * 0.5f, hh = height * 0.5f, hd = depth * 0.5f;

    struct Face {
        glm::vec3 normal;
        glm::vec3 v0, v1, v2, v3;
    };

    std::vector<Face> faces = {
        { { 0,  0,  1}, {-hw,-hh, hd}, { hw,-hh, hd}, { hw, hh, hd}, {-hw, hh, hd} },
        { { 0,  0, -1}, { hw,-hh,-hd}, {-hw,-hh,-hd}, {-hw, hh,-hd}, { hw, hh,-hd} },
        { { 1,  0,  0}, { hw,-hh, hd}, { hw,-hh,-hd}, { hw, hh,-hd}, { hw, hh, hd} },
        { {-1,  0,  0}, {-hw,-hh,-hd}, {-hw,-hh, hd}, {-hw, hh, hd}, {-hw, hh,-hd} },
        { { 0,  1,  0}, {-hw, hh, hd}, { hw, hh, hd}, { hw, hh,-hd}, {-hw, hh,-hd} },
        { { 0, -1,  0}, {-hw,-hh,-hd}, { hw,-hh,-hd}, { hw,-hh, hd}, {-hw,-hh, hd} },
    };

    for (const auto& f : faces) {
        uint32_t base = static_cast<uint32_t>(vertices.size());
        vertices.push_back({ f.v0, f.normal, kDefaultColor, {0.0f, 0.0f} });
        vertices.push_back({ f.v1, f.normal, kDefaultColor, {1.0f, 0.0f} });
        vertices.push_back({ f.v2, f.normal, kDefaultColor, {1.0f, 1.0f} });
        vertices.push_back({ f.v3, f.normal, kDefaultColor, {0.0f, 1.0f} });

        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    return { vertices, indices };
}

MeshData Primitives::generateSphere(float radius, uint32_t radialSegments, uint32_t rings) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (uint32_t ring = 0; ring <= rings; ++ring) {
        float v = static_cast<float>(ring) / rings;
        float phi = v * PI; // 0 (top) -> PI (bottom)
        float sinPhi = std::sin(phi), cosPhi = std::cos(phi);

        for (uint32_t seg = 0; seg <= radialSegments; ++seg) {
            float u = static_cast<float>(seg) / radialSegments;
            float theta = u * 2.0f * PI;
            float sinTheta = std::sin(theta), cosTheta = std::cos(theta);

            glm::vec3 normal = { cosTheta * sinPhi, cosPhi, sinTheta * sinPhi };
            glm::vec3 pos = normal * radius;

            vertices.push_back({ pos, normal, kDefaultColor, { u, v } });
        }
    }

    for (uint32_t ring = 0; ring < rings; ++ring) {
        for (uint32_t seg = 0; seg < radialSegments; ++seg) {
            uint32_t a = ring * (radialSegments + 1) + seg;
            uint32_t b = a + radialSegments + 1;

            indices.push_back(a); indices.push_back(b); indices.push_back(a + 1);
            indices.push_back(b); indices.push_back(b + 1); indices.push_back(a + 1);
        }
    }

    return { vertices, indices };
}

MeshData Primitives::generateCapsule(float radius, float height, uint32_t radialSegments, uint32_t rings) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float cylinderHalf = height * 0.5f;


    for (uint32_t ring = 0; ring <= rings; ++ring) {
        float v = static_cast<float>(ring) / rings;
        float phi = v * (PI * 0.5f);
        float sinPhi = std::sin(phi), cosPhi = std::cos(phi);

        for (uint32_t seg = 0; seg <= radialSegments; ++seg) {
            float u = static_cast<float>(seg) / radialSegments;
            float theta = u * 2.0f * PI;
            float sinTheta = std::sin(theta), cosTheta = std::cos(theta);

            glm::vec3 normal = { cosTheta * sinPhi, cosPhi, sinTheta * sinPhi };
            glm::vec3 pos = normal * radius + glm::vec3(0, cylinderHalf, 0);

            vertices.push_back({ pos, normal, kDefaultColor, { u, v * 0.25f } });
        }
    }

    uint32_t ringCount = radialSegments + 1;
    for (uint32_t ring = 0; ring < rings; ++ring) {
        for (uint32_t seg = 0; seg < radialSegments; ++seg) {
            uint32_t a = ring * ringCount + seg;
            uint32_t b = a + ringCount;
            indices.push_back(a); indices.push_back(a + 1); indices.push_back(b);
            indices.push_back(a + 1); indices.push_back(b + 1); indices.push_back(b);
        }
    }

    uint32_t cylStart = static_cast<uint32_t>(vertices.size());
    for (uint32_t y = 0; y <= 1; ++y) {
        float py = (y == 0) ? cylinderHalf : -cylinderHalf;
        float v = 0.25f + static_cast<float>(y) * 0.5f;
        for (uint32_t seg = 0; seg <= radialSegments; ++seg) {
            float u = static_cast<float>(seg) / radialSegments;
            float theta = u * 2.0f * PI;
            float sinTheta = std::sin(theta), cosTheta = std::cos(theta);
            glm::vec3 normal = { cosTheta, 0, sinTheta };
            glm::vec3 pos = { cosTheta * radius, py, sinTheta * radius };
            vertices.push_back({ pos, normal, kDefaultColor, { u, v } });
        }
    }
    for (uint32_t seg = 0; seg < radialSegments; ++seg) {
        uint32_t a = cylStart + seg;
        uint32_t b = a + radialSegments + 1;
        indices.push_back(a); indices.push_back(a + 1); indices.push_back(b);
        indices.push_back(a + 1); indices.push_back(b + 1); indices.push_back(b);
    }

    uint32_t botStart = static_cast<uint32_t>(vertices.size());
    for (uint32_t ring = 0; ring <= rings; ++ring) {
        float v = static_cast<float>(ring) / rings;
        float phi = PI * 0.5f + v * (PI * 0.5f);
        float sinPhi = std::sin(phi), cosPhi = std::cos(phi);

        for (uint32_t seg = 0; seg <= radialSegments; ++seg) {
            float u = static_cast<float>(seg) / radialSegments;
            float theta = u * 2.0f * PI;
            float sinTheta = std::sin(theta), cosTheta = std::cos(theta);

            glm::vec3 normal = { cosTheta * sinPhi, cosPhi, sinTheta * sinPhi };
            glm::vec3 pos = normal * radius + glm::vec3(0, -cylinderHalf, 0);

            vertices.push_back({ pos, normal, kDefaultColor, { u, 0.75f + v * 0.25f } });
        }
    }
    for (uint32_t ring = 0; ring < rings; ++ring) {
        for (uint32_t seg = 0; seg < radialSegments; ++seg) {
            uint32_t a = botStart + ring * ringCount + seg;
            uint32_t b = a + ringCount;
            indices.push_back(a); indices.push_back(a + 1); indices.push_back(b);
            indices.push_back(a + 1); indices.push_back(b + 1); indices.push_back(b);
        }
    }

    return { vertices, indices };
}

MeshData Primitives::generatePlane(float width, float depth) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float hw = width * 0.5f, hd = depth * 0.5f;
    glm::vec3 n = { 0.0f, 1.0f, 0.0f };

    vertices.push_back({ {-hw, 0.0f,  hd}, n, kDefaultColor, {0.0f, 1.0f} });
    vertices.push_back({ { hw, 0.0f,  hd}, n, kDefaultColor, {1.0f, 1.0f} });
    vertices.push_back({ { hw, 0.0f, -hd}, n, kDefaultColor, {1.0f, 0.0f} });
    vertices.push_back({ {-hw, 0.0f, -hd}, n, kDefaultColor, {0.0f, 0.0f} });

    indices = { 0, 1, 2, 0, 2, 3 };

    return { vertices, indices };
}

MeshData Primitives::generateQuad(float width, float height) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float hw = width * 0.5f, hh = height * 0.5f;
    glm::vec3 n = { 0.0f, 0.0f, 1.0f };

    vertices.push_back({ {-hw, -hh, 0.0f}, n, kDefaultColor, {0.0f, 0.0f} });
    vertices.push_back({ { hw, -hh, 0.0f}, n, kDefaultColor, {1.0f, 0.0f} });
    vertices.push_back({ { hw, hh, 0.0f}, n, kDefaultColor, {1.0f, 1.0f} });
    vertices.push_back({ {-hw, hh, 0.0f}, n, kDefaultColor, {0.0f, 1.0f} });

    indices = { 0, 1, 2, 0, 2, 3 };

    return { vertices, indices };
}

MeshData Primitives::generateCylinder(float topRadius, float bottomRadius, float height, uint32_t radialSegments) {
    QI_CORE_ASSERT(false, "Primitives::generateCylinder is not implemented yet");
    return {};
}

MeshData Primitives::generateTorus(float outerRadius, float innerRadius, uint32_t ringSegments, uint32_t sides) {
    QI_CORE_ASSERT(false, "Primitives::generateTorus is not implemented yet");
    return {};
}

MeshData Primitives::generatePrism(float width, float height, float depth, float leftToRight) {
    QI_CORE_ASSERT(false, "Primitives::generatePrism is not implemented yet");
    return {};
}

}
