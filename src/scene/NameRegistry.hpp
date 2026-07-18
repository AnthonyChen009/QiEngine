#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
namespace Qi {

class NameRegistry {
public:
    std::string makeUniqueName(const std::string& name);
    //TO-DO make sure the user can rename a node to something with the same name
private:
    std::unordered_map<std::string, uint32_t> m_names;

};

}
