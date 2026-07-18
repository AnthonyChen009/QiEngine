#pragma once
#include "types/Vertex.hpp"
#include <glm/glm.hpp>

namespace Qi {

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::string albedoTexturePath;
};

class MeshLoader {

public:
    MeshData getMeshData(const std::string& path);

private:
    MeshData loadObjMeshData(const std::string& path);

};

}
