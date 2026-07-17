#include "core/Log.hpp"
#include "types/VertexHash.hpp"
#include <filesystem>
#include "MeshLoader.hpp"
#include "tiny_obj_loader/tiny_obj_loader.h"

namespace Qi {

MeshData MeshLoader::getMeshData(const std::string& path) {
    std::filesystem::path p(path);
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (!std::filesystem::is_regular_file(path)) {
        QI_CORE_WARN("Mesh file not found or not a regular file: {}", path);
        return MeshData{};
    }

    if (ext == ".obj") {
        return loadObjMeshData(path);
    }
    else {
        QI_CORE_WARN("Unsupported mesh file extension: {}. Mesh loading skipped...", ext);
        return MeshData{};
    }
}

MeshData MeshLoader::loadObjMeshData(const std::string& path) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./";

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config)) {
        if (!reader.Error().empty()) {
            QI_CORE_ERROR("TinyObjReader: {}", reader.Error());
        }
        return MeshData{};
    }

    if (!reader.Warning().empty()) {
        QI_CORE_WARN("TinyObjReader: {}", reader.Warning());
    }

    const tinyobj::attrib_t& attrib = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();

    MeshData meshData;
    std::unordered_map<Vertex, uint32_t, VertexHash> uniqueVertices;

    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                Vertex vertex{};
                vertex.pos = {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                };

                if (idx.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    };
                }

                if (idx.texcoord_index >= 0) {
                    vertex.texCoord = {
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    };
                }

                auto it = uniqueVertices.find(vertex);
                if (it == uniqueVertices.end()) {
                    uint32_t newIndex = static_cast<uint32_t>(meshData.vertices.size());
                    uniqueVertices[vertex] = newIndex;
                    meshData.vertices.push_back(vertex);
                    meshData.indices.push_back(newIndex);
                } else {
                    meshData.indices.push_back(it->second);
                }
            }

            index_offset += fv;
        }
    }

    return meshData;
}

}
