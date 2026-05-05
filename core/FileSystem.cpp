#include "FileSystem.hpp"
#include <fstream>
#include <filesystem>
#include "Log.hpp"
#include "Assert.hpp"

namespace Qi::FileSystem {

std::vector<char> readBinaryFile(const std::string& path) {
    QI_CORE_INFO("Working directory: {0}", std::filesystem::current_path().string());
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        QI_CORE_ERROR("Failed to open file: {0}", path);
        return {};
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    return buffer;
}

std::string readTextFile(const std::string& path) {
    std::ifstream file(path);

    QI_CORE_ASSERT(file.is_open(), "Failed to open text file!");

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

}
