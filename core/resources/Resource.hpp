#pragma once

#include "Base.hpp"
#include <string>

namespace Qi {

class Resource {
public:
    virtual ~Resource() = default;

    const std::string& GetPath() const { return m_Path; }

protected:
    Resource() = default;
    explicit Resource(std::string path) : m_Path(std::move(path)) {}

protected:
    std::string m_Path;
};

}
