#pragma once
#include "core/Base.hpp"
#include "glm/glm.hpp"
#include <vector>
#include <string>

namespace Qi {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
    ~Mesh() = default;

    // File loading
    static Ref<Mesh> load(const std::string& path);

    // Procedural primitives
    static Ref<Mesh> createCube();
    static Ref<Mesh> createSphere(uint32_t subdivisions = 16);
    static Ref<Mesh> createPlane();
    static Ref<Mesh> createQuad();

    // Accessors
    const std::vector<Vertex>&   getVertices() const { return m_vertices; }
    const std::vector<uint32_t>& getIndices()  const { return m_indices;  }

private:
    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;
};

}
