#pragma once
#include <cstdint>

namespace Qi {

using CommandBufferHandle = void*;

class VertexBuffer {
public:
    virtual ~VertexBuffer() = default;

    virtual void bind(CommandBufferHandle commandBuffer) const = 0;
    virtual bool isBufferValid() const = 0;
};

}
