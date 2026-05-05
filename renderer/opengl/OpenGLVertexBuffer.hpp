#pragma once

#include <cstdint>

namespace Qi {

class OpenGLVertexBuffer {
public:
    OpenGLVertexBuffer(const void* data, uint32_t size);
    ~OpenGLVertexBuffer();

    void bind() const;
    void unbind() const;

private:
    uint32_t m_rendererID = 0;
};

}
