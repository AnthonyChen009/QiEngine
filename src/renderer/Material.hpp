#pragma once

#include <string>
#include <cstdint>
#include "types/MaterialParameters.hpp"

namespace Qi {

class Material {
public:
    virtual ~Material() = default;

    virtual const std::string& getPath() const = 0;
    virtual uint32_t getIndex() const = 0;
    virtual const MaterialParameters& getParameters() const = 0;
    virtual void setIndex(uint32_t ind) = 0;
};

}
