#include "core/Log.hpp"
#include <filesystem>
#define TINYOBJLOADER_IMPLEMENTATION
#include "MeshLoader.hpp"
#include "tiny_obj_loader.h"

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
    return MeshData{};
}

}
