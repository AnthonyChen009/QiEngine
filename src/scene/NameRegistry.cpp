#include "NameRegistry.hpp"
#include <string>

namespace Qi {

std::string NameRegistry::makeUniqueName(const std::string& name) {
    uint32_t& count = m_names[name];

    if (count == 0) {
        count = 1;
        return name;
    }

    return name + std::to_string(++count);
}

}
