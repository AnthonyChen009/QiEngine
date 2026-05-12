#pragma once

#include <cstdint>

namespace Qi {

class OpenGLVertexArray {
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray();

    void bind() const;
    void unbind() const;

private:
    uint32_t m_rendererID = 0;
};

}
