#pragma once

#include <string>
#include <cstdint>

namespace Qi {

class Texture2D {
public:
    virtual ~Texture2D() = default;

    static Texture2D* load(const std::string& path);

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual const std::string& getPath() const = 0;
    virtual uint32_t getIndex() const = 0;
};

}
