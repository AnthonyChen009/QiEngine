#pragma once
#include <cstdint>

namespace Qi {

using CommandBufferHandle = void*;

class IndexBuffer {
public:
    virtual ~IndexBuffer() = default;

    virtual void bind(CommandBufferHandle commandBuffer) const = 0;
    virtual uint32_t getCount() const = 0;
    virtual bool isBufferValid() const = 0;
};

}
