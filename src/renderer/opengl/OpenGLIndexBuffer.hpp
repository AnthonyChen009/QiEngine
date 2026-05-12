#pragma once

#include <cstdint>

namespace Qi {

class OpenGLIndexBuffer {
public:
    OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
    ~OpenGLIndexBuffer();

    void bind() const;
    void unbind() const;

    uint32_t getCount() const { return m_count; }

private:
    uint32_t m_rendererID = 0;
    uint32_t m_count = 0;
};

}
