#include "FileSystem.hpp"
#include "core/Log.hpp"
#include "core/Assert.hpp"

#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Qi::FileSystem {

static std::filesystem::path s_executablePath;

void init() {
#ifdef _WIN32

    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    s_executablePath = std::filesystem::path(buffer).parent_path();
#else
    char buffer[4096];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer));

    s_executablePath =
        std::filesystem::path(std::string(buffer, len)).parent_path();
#endif
}

std::filesystem::path getAssetPath() {
    return s_executablePath / "assets";
}

std::filesystem::path getExecutablePath() {
    return s_executablePath;
}

std::filesystem::path resolvePath(const std::filesystem::path& path) {
    return getAssetPath() / path;
}

std::vector<char> readBinaryFile(const std::string& path) {
    auto fullPath = resolvePath(path);

    std::ifstream file(fullPath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        QI_CORE_ERROR("Failed to open file: {}", fullPath.string());
        return {};
    }

    size_t fileSize = static_cast<size_t>(file.tellg());

    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    return buffer;
}

std::string readTextFile(const std::string& path) {
    auto fullPath = resolvePath(path);

    std::ifstream file(fullPath);
    QI_CORE_ERROR("Failed to open text file: {}", fullPath.string());
    QI_CORE_ASSERT(file.is_open(), "Failed to open File");

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

}
