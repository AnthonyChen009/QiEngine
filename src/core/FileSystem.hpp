#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace Qi::FileSystem {

void initialize();

std::filesystem::path getAssetPath();
std::filesystem::path resolvePath(const std::filesystem::path& path);

std::vector<char> readBinaryFile(const std::string& path);
std::string readTextFile(const std::string& path);
std::filesystem::path getExecutablePath();

}
