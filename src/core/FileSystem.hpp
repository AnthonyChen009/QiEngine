#pragma once
#include <vector>
#include <string>

namespace Qi::FileSystem {
    std::vector<char> readBinaryFile(const std::string& path);
    std::string readTextFile(const std::string& path);
}
