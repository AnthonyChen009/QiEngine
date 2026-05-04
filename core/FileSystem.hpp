#pragma once
#include <vector>
#include <string>

namespace Qi::FileSystem {
    std::vector<char> readBinaryFile(const std::string& path);
}
